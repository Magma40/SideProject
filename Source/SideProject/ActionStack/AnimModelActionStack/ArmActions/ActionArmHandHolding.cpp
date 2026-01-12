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

void UActionArmHandHolding::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimLocations.Insert(HandLocationPos, 0);

	const FTransform XformOther = MoveOtherArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPosOther = Xform.InverseTransformPosition(MoveOtherArmRef->ArmParts[2]->GetComponentLocation());
	AnimLocations.Insert(HandLocationPosOther, 1);
}

void UActionArmHandHolding::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}
	const TTuple<TTuple<TArray<FVector>, float>,FVector> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimLocations, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value;

	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimLocations[0] = HandLocationPos;

	const FTransform XformOther = MoveOtherArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPosOther = Xform.InverseTransformPosition(MoveOtherArmRef->ArmParts[2]->GetComponentLocation());
	AnimLocations[1]  = HandLocationPosOther;
}

bool UActionArmHandHolding::IsDone_Implementation() const
{
	return false;
}
