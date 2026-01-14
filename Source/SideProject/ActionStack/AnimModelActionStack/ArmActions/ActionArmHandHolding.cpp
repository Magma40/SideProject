// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandHolding.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmHandHolding* UActionArmHandHolding::Create(UObject* Outer, UMoveArm* InMoveArm, UMoveArm* InOtherMoveArm, ABaseInteractableCharacter* InCharacter, ABaseInteractableCharacter* InAICharacter)
{
	UActionArmHandHolding* Action = NewObject<UActionArmHandHolding>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->MoveOtherArmRef = InOtherMoveArm;
	Action->OwnerCharacter = InCharacter;
	Action->AIOwnerCharacter = InAICharacter;
	return Action;
}

void UActionArmHandHolding::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));

	const FVector HandLocationPosOther = Xform.InverseTransformPosition(MoveOtherArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPosOther));
	MoveArmRef->bEnableArmConstraints = false;
}

void UActionArmHandHolding::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	MoveArmRef->bEnableArmConstraints = true;
}

void UActionArmHandHolding::OnUpdate_Implementation(const float DeltaTime) 
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

	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms[0].SetLocation(HandLocationPos);

	FVector HandLocationPosOther = Xform.InverseTransformPosition(MoveOtherArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms[1].SetLocation(HandLocationPosOther);
}

bool UActionArmHandHolding::IsDone_Implementation() const
{
	return false;
}
