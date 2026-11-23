#include "SwitchCameraPointActor.h"
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

ASwitchCameraPointActor::ASwitchCameraPointActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(RootComponent);

	//Positive Y Box
	PositiveBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PositiveBoxCollider"));
	PositiveBoxComponent->SetupAttachment(RootComponent);
	PositiveBoxComponent->InitBoxExtent(FVector(50.f));
	PositiveBoxComponent->SetCollisionProfileName(TEXT("Trigger"));
	PositiveBoxComponent->SetCanEverAffectNavigation(false);

	//Negative Y Box
	NegativeBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("NegativeBoxCollider"));
	NegativeBoxComponent->SetupAttachment(RootComponent);
	NegativeBoxComponent->InitBoxExtent(FVector(50.f));
	NegativeBoxComponent->SetCollisionProfileName(TEXT("Trigger"));
	NegativeBoxComponent->SetCanEverAffectNavigation(false);

	//Distance from center (half width of your actor in Y)
	const float YOffset = 200.f;

	PositiveBoxComponent->SetRelativeLocation(FVector(0.f,  YOffset, 0.f));
	NegativeBoxComponent->SetRelativeLocation(FVector(0.f, -YOffset, 0.f));
}

void ASwitchCameraPointActor::BeginPlay()
{
	Super::BeginPlay();
	
	PositiveBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASwitchCameraPointActor::PositiveOverlapBegin);
	PositiveBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ASwitchCameraPointActor::PositiveOverlapEnd);

	NegativeBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASwitchCameraPointActor::NegativeOverlapBegin);
	NegativeBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ASwitchCameraPointActor::NegativeOverlapEnd);

	const APlayerCamera* PlayerCamera = Cast<APlayerCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass()));
	
	if (!IsValid(PlayerCamera))
	{
		return;
	}
	
	// TArray<UCameraActionStack* >Components;
	// PlayerCamera->GetComponents<UCameraActionStack>(Components, true);
	//
	// if (!Components.IsEmpty() &&
	// 	IsValid(Components[0]))
	// {
	// 	CameraActionStack = Components[0];
	// }
	
	if (!IsValid(CameraActionStack))
	{
		return;
	}
}

void ASwitchCameraPointActor::PositiveOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
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
	
	if (!IsValid(CameraPoint_L))
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
	
	if (bDontDoIfJumping && 
		TempChar->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	bHasTriggered = true;

	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ACompanionPawn::StaticClass()));
	
	if (!IsValid(CompanionPawn))
	{
		return;
	}
	
	// if (UActionToCameraPoint* ActionRef = CameraActionStack->GetAction<UActionToCameraPoint>())
	// {
	// 	CameraActionStack->RemoveAction(ActionRef);
	// }
	
	CameraActionStack->PushToCameraPointAction(CameraPoint_L, PlayRate);
	
	CompanionPawn->SetNewRespawnPoint(CameraPoint_L->CompanionRoomSpawnPoint);
	
	TempChar->SetNewRespawnPoint(CameraPoint_L->PlayerRoomSpawnPoint);
	
	// if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>())
	// {
	// 	ActionRef->SetRotationLimit(CameraPoint_L->RotationLimit);
	// }
}

void ASwitchCameraPointActor::PositiveOverlapEnd(UPrimitiveComponent* OverlappedComponent, 
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

void ASwitchCameraPointActor::NegativeOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
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
	
	if (!IsValid(CameraPoint_R))
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
	
	if (bDontDoIfJumping && TempChar->GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	bHasTriggered = true;

	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ACompanionPawn::StaticClass()));
	
	if (!IsValid(CompanionPawn))
	{
		return;
	}
	
	// if (UActionToCameraPoint* ActionRef = CameraActionStack->GetAction<UActionToCameraPoint>())
	// {
	// 	CameraActionStack->RemoveAction(ActionRef);
	// }
	
	CameraActionStack->PushToCameraPointAction(CameraPoint_R, PlayRate);
	
	CompanionPawn->SetNewRespawnPoint(CameraPoint_R->CompanionRoomSpawnPoint);
	
	TempChar->SetNewRespawnPoint(CameraPoint_R->PlayerRoomSpawnPoint);
	
	// if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>())
	// {
	// 	ActionRef->SetRotationLimit(CameraPoint_R->RotationLimit);
	// }
}

void ASwitchCameraPointActor::NegativeOverlapEnd(UPrimitiveComponent* OverlappedComponent,
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
