// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "WorldObjects/PlayerCamera.h"

UActionToCameraPoint* UActionToCameraPoint::Create(UObject* Outer, APlayerCamera* InPlayerCamera)
{
	UActionToCameraPoint* Action = NewObject<UActionToCameraPoint>(Outer);
	Action->PlayerCamera = InPlayerCamera;
	return Action;
}

void UActionToCameraPoint::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	AnimTransforms.Add(PlayerCamera->GetActorTransform());
	AnimTransforms.Add(PlayerCamera->GetStartCameraTransform());
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
