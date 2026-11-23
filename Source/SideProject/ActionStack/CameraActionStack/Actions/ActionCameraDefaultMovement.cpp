#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/InputSettings.h"
#include "Player/MainCharacter.h"
#include "Public/CameraPoint.h"
#include "WorldObjects/PlayerCamera.h"

void UActionCameraDefaultMovement::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(OwnerCharacter));

	const APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	check(IsValid(PC));

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PC->InputComponent);
	check(IsValid(Input));

	Input->BindAction(OwnerCharacter->GetLookAction(), ETriggerEvent::Triggered, this, &UActionCameraDefaultMovement::LimitLookRangeForCamera);
	Input->BindAction(OwnerCharacter->GetMoveAction(), ETriggerEvent::Triggered, this, &UActionCameraDefaultMovement::LimitMoveRangeForCamera);
}

void UActionCameraDefaultMovement::LimitLookRangeForCamera(const FInputActionValue& Value)
{
	check(IsValid(PlayerCamera));
	check(IsValid(PlayerController));
	check(IsValid(InputDeviceSubsystem));
	
	const FRotator CameraRotation = PlayerCamera->GetActorRotation();
	const FRotator StartTransformRotation = PlayerCamera->GetStartCameraTransform().GetRotation().Rotator();
	FVector2D MovementVector = Value.Get<FVector2D>();

	//If Player using is a gamepad, invert Y 
    const FPlatformUserId UserId = PlayerController->GetPlatformUserId();
    const FHardwareDeviceIdentifier DeviceIdentifier = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(UserId);
	
    if (DeviceIdentifier.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
    {
    	MovementVector.Y *= -1.f;
    }

	//Calculate new rotation for camera
	const float DesiredPitch = CameraRotation.Pitch + MovementVector.Y * GetWorld()->GetDeltaSeconds() * 50.0f;
	const float DesiredYaw   = CameraRotation.Yaw   + MovementVector.X * GetWorld()->GetDeltaSeconds() * 50.0f;

	float PitchDelta = FMath::FindDeltaAngleDegrees(StartTransformRotation.Pitch, DesiredPitch);
	float YawDelta   = FMath::FindDeltaAngleDegrees(StartTransformRotation.Yaw, DesiredYaw);

	//Clamp rotation so camera has limited range
	PitchDelta = FMath::Clamp(PitchDelta, -RotationLimit, RotationLimit);
	YawDelta   = FMath::Clamp(YawDelta,   -RotationLimit, RotationLimit);

	//Apply new rotation
	const FRotator LimitedRotation(
		StartTransformRotation.Pitch + PitchDelta, 
		StartTransformRotation.Yaw   + YawDelta, 
		StartTransformRotation.Roll);
	
	PlayerCamera->SetActorRelativeRotation(LimitedRotation);
}

void UActionCameraDefaultMovement::LimitMoveRangeForCamera()
{
	check(IsValid(PlayerCamera));
	
	const ACameraPoint* CurrentPoint = PlayerCamera->GetCurrentCameraPoint();
	check(IsValid(PlayerCamera));

    const FTransform BaseTransform = PlayerCamera->GetStartCameraTransform();
	const FVector BaseLocation = BaseTransform.GetLocation();

	//Get the forward of the current camera point
    const FRotator BaseRot = BaseTransform.GetRotation().Rotator();
    const FRotator YawRotation(0.f, BaseRot.Yaw, 0.f);

	//Get the right of the current camera point
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//Calculate if player is in range within camera and is not over camera slider range
	float TargetOffset = FVector::DotProduct(OwnerCharacter->GetActorLocation() - BaseLocation, RightDirection);
	TargetOffset = FMath::Clamp(TargetOffset,  -CurrentPoint->MinMaxSliderValue,  CurrentPoint->MinMaxSliderValue);

	//Apply new position
    const FVector NewLocation = BaseLocation + RightDirection * TargetOffset;
    PlayerCamera->SetActorLocation(NewLocation);
}