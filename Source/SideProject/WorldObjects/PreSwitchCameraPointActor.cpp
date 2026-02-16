#include "PreSwitchCameraPointActor.h"

#include "CameraPoint.h"
#include "PlayerCamera.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Object.h"
#include "Player/MainCharacter.h"

APreSwitchCameraPointActor::APreSwitchCameraPointActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	SetRootComponent(RootComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->InitBoxExtent(FVector(50.f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
	BoxComponent->SetCanEverAffectNavigation(false);
}

void APreSwitchCameraPointActor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APreSwitchCameraPointActor::OverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APreSwitchCameraPointActor::OverlapEnd);

	PlayerCamera = Cast<APlayerCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass()));
	if (!IsValid(PlayerCamera)) return;
	
	TArray<UCameraActionStack* >Components;
	PlayerCamera->GetComponents<UCameraActionStack>(Components, true);
	if (!Components.IsEmpty() && IsValid(Components[0])) CameraActionStack = Components[0];
	if (!IsValid(CameraActionStack)) return;
}

void APreSwitchCameraPointActor::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(InsideAreaCameraPoint) || !IsValid(OutsideAreaCameraPoint)) return;
	if (bHasTriggered) return;

	AMainCharacter* TempChar = Cast<AMainCharacter>(OtherActor);
	if (!TempChar) return;
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent()) return;
	
	bHasTriggered = true;

	if (TempChar->GetCharacterMovement()->IsFalling())
	{
		if (bDontDoIfJumping) return;
		else
		{
			CameraActionStack->PushToCameraPointAction(OutsideAreaCameraPoint, PlayRate);
			if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>()) ActionRef->SetRotationLimit(OutsideAreaCameraPoint->RotationLimit);
		}
	}
	
	if (UActionToCameraPoint* ActionRef = CameraActionStack->GetAction<UActionToCameraPoint>()) CameraActionStack->RemoveAction(ActionRef);
	CameraActionStack->PushToCameraPointAction(InsideAreaCameraPoint, PlayRate);
	if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>()) ActionRef->SetRotationLimit(InsideAreaCameraPoint->RotationLimit);
}

void APreSwitchCameraPointActor::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(InsideAreaCameraPoint) || !IsValid(OutsideAreaCameraPoint)) return;
	AMainCharacter* TempChar = Cast<AMainCharacter>(OtherActor);
	if (!TempChar) return;
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent()) return;

	for (ACameraPoint* CameraPoint : ExceptionalCameraPoints)
	{
		if (!IsValid(CameraPoint)) continue;
		if (PlayerCamera->GetCurrentCameraPoint() == CameraPoint) return;
	}

	if (UActionToCameraPoint* ActionRef = CameraActionStack->GetAction<UActionToCameraPoint>()) CameraActionStack->RemoveAction(ActionRef);
	CameraActionStack->PushToCameraPointAction(OutsideAreaCameraPoint, PlayRate);
	if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>()) ActionRef->SetRotationLimit(OutsideAreaCameraPoint->RotationLimit);
	
	bHasTriggered = false;
}
