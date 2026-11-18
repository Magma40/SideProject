#include "FallingSwitchCameraPointActor.h"
#include "CameraPoint.h"
#include "PlayerCamera.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CompanionPawn.h"
#include "UObject/Object.h"
#include "Player/MainCharacter.h"

AFallingSwitchCameraPointActor::AFallingSwitchCameraPointActor()
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

void AFallingSwitchCameraPointActor::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFallingSwitchCameraPointActor::OverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AFallingSwitchCameraPointActor::OverlapEnd);

	const APlayerCamera* PlayerCamera = Cast<APlayerCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass()));
	
	if (!IsValid(PlayerCamera))
	{
		return;
	}
	
	TArray<UCameraActionStack* >Components;
	PlayerCamera->GetComponents<UCameraActionStack>(Components, true);
	
	if (!Components.IsEmpty() && IsValid(Components[0]))
	{
		CameraActionStack = Components[0];
	}
	
	if (!IsValid(CameraActionStack))
	{
		return;
	}
}

void AFallingSwitchCameraPointActor::OverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex,
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (bHasTriggered)
	{
		return;
	}

	AMainCharacter* TempChar = Cast<AMainCharacter>(OtherActor);
	
	if (!TempChar)
	{
		return;
	}
	
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent())
	{
		return;
	}
	
	bHasTriggered = true;

	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ACompanionPawn::StaticClass()));
	
	if (!IsValid(CompanionPawn))
	{
		return;
	}

	if (UActionToCameraPoint* ActionRef = CameraActionStack->GetAction<UActionToCameraPoint>())
	{
		CameraActionStack->RemoveAction(ActionRef);
	}
	
	if (TempChar->GetCharacterMovement()->IsFalling())
	{
		CameraActionStack->PushToCameraPointAction(FallBackCameraPoint, PlayRate);
		
		CompanionPawn->SetNewRespawnPoint(FallBackCameraPoint->CompanionRoomSpawnPoint);
		
		TempChar->SetNewRespawnPoint(FallBackCameraPoint->PlayerRoomSpawnPoint);
		
		if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>())
		{
			ActionRef->SetRotationLimit(FallBackCameraPoint->RotationLimit);
		}
	}
}

void AFallingSwitchCameraPointActor::OverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	const AMainCharacter* TempChar = Cast<AMainCharacter>(OtherActor);
	
	if (!TempChar)
	{
		return;
	}
	
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent())
	{
		return;
	}

	bHasTriggered = false;
}
