// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainCharacter.h"
#include "Public/CameraPoint.h"
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

	Input->BindAction(OwnerCharacter->GetLookAction(), ETriggerEvent::Triggered, this, &UActionCameraDefaultMovement::LimitLookRangeForCamera);
	Input->BindAction(OwnerCharacter->GetMoveAction(), ETriggerEvent::Triggered, this, &UActionCameraDefaultMovement::LimitMoveRangeForCamera);
}

void UActionCameraDefaultMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	// const FVector StartTransformLocation = PlayerCamera->GetStartCameraTransform().GetLocation();
	// const FVector CurrentLocation = PlayerCamera->GetActorLocation();
	// const FVector PlayerPos = OwnerCharacter->GetActorLocation();
	//
	// const FVector GroundPos(CurrentLocation.X, CurrentLocation.Y, OwnerCharacter->GetActorLocation().Z);
	// const FRotator CameraRot = StartTransformLocation.Rotation();
	// const FRotator YawRotation(0, CameraRot.Yaw, 0);
	// const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// const FVector ForwardDirection = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//
	// const float SideOffset = 250.f; // width of the cone/box
	// const float Distance = FVector::Dist2D(PlayerPos, CurrentLocation);
	// const FVector End = GroundPos + ForwardDirection * Distance;
	// const FVector EndRight = End + (RightDirection * SideOffset);
	// const FVector EndLeft  = End - (RightDirection * SideOffset);
	// const FVector BeginRight = GroundPos + (RightDirection * SideOffset);
	// const FVector BeginLeft  = GroundPos - (RightDirection * SideOffset);
	//
	// DrawDebugLine(GetWorld(), GroundPos, End, FColor::Cyan, false, 0.f, 0, 2.f);
	// DrawDebugLine(GetWorld(), BeginRight, EndRight, FColor::Cyan, false, 0.f, 0, 2.f);
	// DrawDebugLine(GetWorld(), BeginLeft, EndLeft, FColor::Cyan, false, 0.f, 0, 2.f);
	// DrawDebugLine(GetWorld(), EndLeft, End, FColor::Cyan, false, 0.f, 0, 2.f);
	// DrawDebugLine(GetWorld(), EndRight, End, FColor::Cyan, false, 0.f, 0, 2.f);
}

bool UActionCameraDefaultMovement::IsDone_Implementation() const
{
	return false;
}

void UActionCameraDefaultMovement::LimitLookRangeForCamera(const FInputActionValue& Value) const // Const function :)
{
	if (!IsValid(PlayerCamera)) return;
	if (!IsValid(PlayerCamera->GetCurrentCameraPoint())) return;
	const FRotator CameraRotation = PlayerCamera->GetActorRotation();
	const FRotator StartTransformRotation = PlayerCamera->GetStartCameraTransform().GetRotation().Rotator();
	FVector2D MovementVector = Value.Get<FVector2D>();

	//If Player using is a gamepad, invert Y 
	if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (UInputDeviceSubsystem* InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>())
    	{
    		const FPlatformUserId UserId = PlayerController->GetPlatformUserId();
    		FHardwareDeviceIdentifier DeviceIdentifier = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(UserId);
    		if (DeviceIdentifier.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad) MovementVector.Y *= -1.f;
    	}
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
	const FRotator LimitedRotation( StartTransformRotation.Pitch + PitchDelta, StartTransformRotation.Yaw   + YawDelta, StartTransformRotation.Roll);
	PlayerCamera->SetActorRelativeRotation(LimitedRotation);
}

void UActionCameraDefaultMovement::LimitMoveRangeForCamera() const
{
    if (!IsValid(PlayerCamera)) return;

    ACameraPoint* CurrentPoint = PlayerCamera->GetCurrentCameraPoint();
    if (!IsValid(CurrentPoint)) return;

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
