#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"

void UActionArmHandToLocation::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(MoveArmRef));
	check(!MoveArmRef->ArmParts.IsEmpty())
	
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));

	const FVector ToLocationPos = Xform.InverseTransformPosition(ToLocation);
	
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, ToLocationPos));

	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
}

void UActionArmHandToLocation::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MoveArmRef));
	check(IsValid(AnimSubsystem));
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms, 
			ElapsedTime,
			PlayRate);
	
	ElapsedTime = NewValues.Key.Value;
	AnimTransforms = NewValues.Key.Key;
	
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
}

bool UActionArmHandToLocation::IsDone() const
{
	return AnimTransforms.Num() <= 1;
}