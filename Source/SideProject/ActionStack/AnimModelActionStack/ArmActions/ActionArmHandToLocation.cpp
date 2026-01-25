// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmHandToLocation* UActionArmHandToLocation::Create(UObject* Outer, UMoveArm* InMoveArm, const FVector& InToLocation)
{
	UActionArmHandToLocation* Action = NewObject<UActionArmHandToLocation>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->ToLocation = InToLocation;
	return Action;
}

void UActionArmHandToLocation::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));

	const FVector ToLocationPos = Xform.InverseTransformPosition(ToLocation);
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, ToLocationPos));
}

void UActionArmHandToLocation::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);

	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	ElapsedTime = NewValues.Key.Value;
	AnimTransforms = NewValues.Key.Key;
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
}

bool UActionArmHandToLocation::IsDone_Implementation() const
{
	return AnimTransforms.Num() <= 1;
}
