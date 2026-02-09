#include "SwitchCameraPointActor.h"

#include "PlayerCamera.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Object.h"
#include "Player/MainCharacter.h"

ASwitchCameraPointActor::ASwitchCameraPointActor()
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

void ASwitchCameraPointActor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASwitchCameraPointActor::OverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ASwitchCameraPointActor::OverlapEnd);

	APlayerCamera* PlayerCamera = Cast<APlayerCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass()));
	if (!IsValid(PlayerCamera)) return;
	
	TArray<UCameraActionStack* >Components;
	PlayerCamera->GetComponents<UCameraActionStack>(Components, true);
	if (!Components.IsEmpty() && IsValid(Components[0])) CameraActionStack = Components[0];
	if (!IsValid(CameraActionStack)) return;
}

void ASwitchCameraPointActor::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasTriggered) return;

	AMainCharacter* TempChar = Cast<AMainCharacter>(OtherActor);
	if (!TempChar) return;
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent()) return;
	
	bHasTriggered = true;
	
	if (TempChar->GetCameraActor()->GetCurrentCameraPoint() == CameraPoint_L)
	{
		CameraActionStack->PushToCameraPointAction(CameraPoint_R);
	}
	else if (TempChar->GetCameraActor()->GetCurrentCameraPoint() == CameraPoint_R)
	{
		CameraActionStack->PushToCameraPointAction(CameraPoint_L);
	}
}

void ASwitchCameraPointActor::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* TempChar = Cast<AMainCharacter>(OtherActor);
	if (!TempChar) return;
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent()) return;

	bHasTriggered = false;
}
