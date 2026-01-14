// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmWave.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmWave* UActionArmWave::Create(UObject* Outer, UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter)
{
	UActionArmWave* Action = NewObject<UActionArmWave>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->OwnerCharacter = InCharacter;
	return Action;
}

void UActionArmWave::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	const FVector MidOffset = HandLocationPos + FVector(-30.f, -25.f, 125.f); // offset
	const FVector MidOffset2 = HandLocationPos + FVector(+20.f, -25.f, 125.f); // offset
	const FVector MidOffset3 = HandLocationPos + FVector(0.0f, 0.0f, 125.f); // offset


	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, MidOffset));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, MidOffset2));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, MidOffset3));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
	MoveArmRef->bEnableArmConstraints = false;
}

void UActionArmWave::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	MoveArmRef->bEnableArmConstraints = true;
}

void UActionArmWave::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}

	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
}

bool UActionArmWave::IsDone_Implementation() const
{
	return AnimTransforms.Num() <= 1;
}
