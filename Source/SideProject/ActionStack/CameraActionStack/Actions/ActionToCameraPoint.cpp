// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Public/CameraPoint.h"
#include "WorldObjects/CameraManagerMain.h"
#include "WorldObjects/PlayerCamera.h"

UActionToCameraPoint* UActionToCameraPoint::Create(UObject* Outer, APlayerCamera* InPlayerCamera, AMainCharacter* InOwnerCharacter, ACameraPoint* InNewCameraPoint)
{
	UActionToCameraPoint* Action = NewObject<UActionToCameraPoint>(Outer);
	Action->PlayerCamera = InPlayerCamera;
	Action->NewCameraPoint = InNewCameraPoint;
	Action->OwnerCharacter = InOwnerCharacter;
	return Action;
}

void UActionToCameraPoint::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	PlayerCamera->SetNewCameraPoint(NewCameraPoint);
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraManagerMain::StaticClass(), AllActors);
	if (AllActors.IsEmpty()) return;
	
	const FTransform BaseTransform = PlayerCamera->GetStartCameraTransform();
	const FVector BaseLocation = BaseTransform.GetLocation();
	const FRotator BaseRotation = BaseTransform.GetRotation().Rotator();

	const FRotator YawRotation(0.f, BaseRotation.Yaw, 0.f);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	float SliderLimit;
	float EntryOffset;
	if (ACameraManagerMain* CameraManager = Cast<ACameraManagerMain>(AllActors[0]))
	{
		if (IsValid(CameraManager->BaseCameraPoint))
		{
			SliderLimit = PlayerCamera->GetCurrentCameraPoint()->MinMaxSliderValue;
			EntryOffset = -SliderLimit;
			CameraManager->BaseCameraPoint = nullptr;
		}
		else
		{
			SliderLimit = PlayerCamera->GetCurrentCameraPoint()->MinMaxSliderValue;
			const FVector PreviousLocation = PlayerCamera->GetActorLocation();
			const float Side = FVector::DotProduct(PreviousLocation - BaseLocation,RightDirection);

			EntryOffset = FMath::Clamp(Side,-SliderLimit,SliderLimit);		
		}
	}
	
	const FVector NewLocation = BaseLocation + RightDirection * EntryOffset;
	const FTransform NewCameraTransform(BaseTransform.GetRotation(),NewLocation,FVector::OneVector);

	AnimTransforms.Add(PlayerCamera->GetActorTransform());
	AnimTransforms.Add(NewCameraTransform);
}

void UActionToCameraPoint::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);

	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}

	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	PlayerCamera->SetActorLocationAndRotation(NewValues.Value.GetLocation(), NewValues.Value.GetRotation());
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
}

bool UActionToCameraPoint::IsDone_Implementation() const
{
	return AnimTransforms.Num() <= 1;
}
