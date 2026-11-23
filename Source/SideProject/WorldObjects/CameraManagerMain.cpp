#include "CameraManagerMain.h"
#include "CameraPoint.h"
#include "PlayerCamera.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CompanionPawn.h"
#include "Player/MainCharacter.h"

ACameraManagerMain::ACameraManagerMain()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACameraManagerMain::BeginPlay()
{
	if (AActor* Camera = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass()); IsValid(Camera))
	{
		if (APlayerCamera* CameraActor = Cast<APlayerCamera>(Camera))
		{
			PlayerCamera = CameraActor;
		}
	}
	
	if (!IsValid(PlayerCamera))
	{
		return;
	}
	//
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

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(PlayerCamera);
	
	if (IsValid(BaseCameraPoint))
	{
		CameraActionStack->PushToCameraPointAction(BaseCameraPoint, 1.0f);
	}
	
	// if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>())
	// {
	// 	ActionRef->SetRotationLimit(BaseCameraPoint->RotationLimit);
	// }

	AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));
	
	if (!IsValid(Player))
	{
		return;
	}

	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ACompanionPawn::StaticClass()));
	
	if (!IsValid(CompanionPawn))
	{
		return;
	}
	
	CompanionPawn->SetNewRespawnPoint(BaseCameraPoint->CompanionRoomSpawnPoint);
	Player->SetNewRespawnPoint(BaseCameraPoint->PlayerRoomSpawnPoint);
}
