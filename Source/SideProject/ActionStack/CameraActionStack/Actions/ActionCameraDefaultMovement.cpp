// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/MainCharacter.h"
#include "WorldObjects/PlayerCamera.h"

UActionCameraDefaultMovement* UActionCameraDefaultMovement::Create(UObject* Outer, AMainCharacter* InOwnerCharacter, APlayerCamera* InPlayerCamera)
{
	UActionCameraDefaultMovement* Action = NewObject<UActionCameraDefaultMovement>(Outer);
	Action->OwnerCharacter = InOwnerCharacter;
	Action->PlayerCamera = InPlayerCamera;
	return Action;
}

void UActionCameraDefaultMovement::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!OwnerCharacter) return;

	const APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PC->InputComponent);
	if (!Input) return;

	Input->BindAction(OwnerCharacter->GetMouseLookAction(), ETriggerEvent::Triggered, this, &UActionCameraDefaultMovement::LimitLookRangeForCamera);
	Input->BindAction(OwnerCharacter->GetMoveAction(), ETriggerEvent::Triggered, this, &UActionCameraDefaultMovement::LimitMoveRangeForCamera);
}

void UActionCameraDefaultMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionCameraDefaultMovement::IsDone_Implementation() const
{
	return false;
}

void UActionCameraDefaultMovement::LimitLookRangeForCamera(const FInputActionValue& Value) // Const function :)
{
	if (!IsValid(PlayerCamera)) return;
	if (!IsValid(PlayerCamera->GetCurrentCameraPoint())) return;
	const FRotator CameraRotation = PlayerCamera->GetActorRotation();
	const FRotator StartTransformRotation = PlayerCamera->GetStartCameraTransform().GetRotation().Rotator();
	const FVector2D MovementVector = Value.Get<FVector2D>();

    float DesiredPitch = CameraRotation.Pitch + MovementVector.Y * GetWorld()->GetDeltaSeconds() * 50.0f * -1.f;
	float DesiredYaw   = CameraRotation.Yaw   + MovementVector.X * GetWorld()->GetDeltaSeconds() * 50.0f;

	DesiredPitch = FMath::Clamp(DesiredPitch, StartTransformRotation.Pitch - RotationLimit, StartTransformRotation.Pitch + RotationLimit);
	DesiredYaw = FMath::Clamp(DesiredYaw,StartTransformRotation.Yaw - RotationLimit, StartTransformRotation.Yaw + RotationLimit);

	const FRotator LimitedRotation(DesiredPitch, DesiredYaw, StartTransformRotation.Roll);
	PlayerCamera->SetActorRelativeRotation(LimitedRotation);
}

void UActionCameraDefaultMovement::LimitMoveRangeForCamera(const FInputActionValue& Value)
{
	if (!IsValid(PlayerCamera)) return;
	if (!IsValid(PlayerCamera->GetCurrentCameraPoint())) return;
	const FVector2D Input = Value.Get<FVector2D>();
	const FVector StartTransformLocation = PlayerCamera->GetStartCameraTransform().GetLocation();
	const FVector CurrentLocation = PlayerCamera->GetActorLocation();

	// Apply input only to X
	float DesiredX = CurrentLocation.X + (Input.X * -1) * OwnerCharacter->GetMovementComponent()->GetMaxSpeed() * GetWorld()->GetDeltaSeconds();

	// Clamp relative to start position
	DesiredX = FMath::Clamp(DesiredX,   StartTransformLocation.X + MinSliderValue, StartTransformLocation.X + MaxSliderValue );

	// Rebuild location: X changes, Y/Z fixed
	const FVector NewLocation( DesiredX, StartTransformLocation.Y, StartTransformLocation.Z);
	PlayerCamera->SetActorLocation(NewLocation);
}
