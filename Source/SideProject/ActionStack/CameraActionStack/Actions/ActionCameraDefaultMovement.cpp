// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	const FVector StartTransformLocation = PlayerCamera->GetStartCameraTransform().GetLocation();
	const FVector CurrentLocation = PlayerCamera->GetActorLocation();
	const FVector PlayerPos = OwnerCharacter->GetActorLocation();
	
	const FVector GroundPos(CurrentLocation.X, CurrentLocation.Y, OwnerCharacter->GetActorLocation().Z);
	const FRotator CameraRot = StartTransformLocation.Rotation();
	const FRotator YawRotation(0, CameraRot.Yaw, 0);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector ForwardDirection = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	const float SideOffset = 250.f; // width of the cone/box
	const float Distance = FVector::Dist2D(PlayerPos, CurrentLocation);
	const FVector End = GroundPos + ForwardDirection * Distance;
	const FVector EndRight = End + (RightDirection * SideOffset);
	const FVector EndLeft  = End - (RightDirection * SideOffset);
	const FVector BeginRight = GroundPos + (RightDirection * SideOffset);
	const FVector BeginLeft  = GroundPos - (RightDirection * SideOffset);

	DrawDebugLine(GetWorld(), GroundPos, End, FColor::Cyan, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), BeginRight, EndRight, FColor::Cyan, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), BeginLeft, EndLeft, FColor::Cyan, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), EndLeft, End, FColor::Cyan, false, 0.f, 0, 2.f);
	DrawDebugLine(GetWorld(), EndRight, End, FColor::Cyan, false, 0.f, 0, 2.f);

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
	const FVector StartTransformLocation = PlayerCamera->GetStartCameraTransform().GetLocation();
	const FVector CurrentLocation = PlayerCamera->GetActorLocation();
	const FVector PlayerPos = OwnerCharacter->GetActorLocation();
	
	// Ground reference
	const FVector GroundPos(CurrentLocation.X, CurrentLocation.Y, OwnerCharacter->GetActorLocation().Z);
	const FRotator CameraRot = StartTransformLocation.Rotation();
	const FRotator YawRotation(0, CameraRot.Yaw, 0);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const float SideOffset = 250.f; // width of the cone/box
	const float Distance = FVector::DotProduct(PlayerPos, CurrentLocation);
	const FVector End = GroundPos + ForwardDirection * Distance;
	if (FMath::Abs(FVector::DotProduct(PlayerPos - End, RightDirection)) < SideOffset)
	{
		const FVector Velocity = OwnerCharacter->GetCharacterMovement()->GetLastUpdateVelocity();
        const float DirectionSpeed = FVector::DotProduct(Velocity.GetSafeNormal(),RightDirection);
		const FVector DesiredLocation = CurrentLocation + RightDirection * DirectionSpeed * OwnerCharacter->GetMovementComponent()->GetMaxSpeed() * GetWorld()->GetDeltaSeconds();
		float Offset = FVector::DotProduct(DesiredLocation - StartTransformLocation, RightDirection);
		Offset = FMath::Clamp(Offset, MinSliderValue, MaxSliderValue);

		const FVector NewLocation = StartTransformLocation + RightDirection * Offset;
		PlayerCamera->SetActorLocation(NewLocation);
	}
}
