#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "Public/CameraPoint.h"
#include "WorldObjects/CameraManagerMain.h"
#include "WorldObjects/PlayerCamera.h"

void UActionToCameraPoint::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(PlayerCamera));
	check(IsValid(NewCameraPoint));
	check(IsValid(CameraManager))
	
	PlayerCamera->SetNewCameraPoint(NewCameraPoint);
	
	const FTransform BaseTransform = PlayerCamera->GetStartCameraTransform();
	const FVector BaseLocation = BaseTransform.GetLocation();
	const FRotator BaseRotation = BaseTransform.GetRotation().Rotator();

	const FRotator YawRotation(0.f, BaseRotation.Yaw, 0.f);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	float SliderLimit;
	float EntryOffset;
	
	if (IsValid(CameraManager->BaseCameraPoint))
	{
		SliderLimit = NewCameraPoint->MinMaxSliderValue;
		EntryOffset = -SliderLimit;
		CameraManager->BaseCameraPoint = nullptr;
	}
	else
	{
		SliderLimit = NewCameraPoint->MinMaxSliderValue;
		const FVector PreviousLocation = PlayerCamera->GetActorLocation();
		const float Side = FVector::DotProduct(PreviousLocation - BaseLocation,RightDirection);

		EntryOffset = FMath::Clamp(Side,-SliderLimit,SliderLimit);		
	}
	
	const FVector NewLocation = BaseLocation + RightDirection * EntryOffset;
	const FTransform NewCameraTransform(
		BaseTransform.GetRotation(),
		NewLocation,
		FVector::OneVector);

	AnimTransforms.Add(PlayerCamera->GetActorTransform());
	AnimTransforms.Add(NewCameraTransform);
}

void UActionToCameraPoint::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(PlayerCamera));
	check(IsValid(AnimSubsystem));

	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms, 
			ElapsedTime, 
			PlayRate);
	
	PlayerCamera->SetActorLocationAndRotation(NewValues.Value.GetLocation(), NewValues.Value.GetRotation());
	
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
}

bool UActionToCameraPoint::IsDone() const
{
	return AnimTransforms.Num() <= 1;
}