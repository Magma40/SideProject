#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandHolding.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"

void UActionArmHandHolding::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(MoveArmRef));
	check(!MoveArmRef->ArmParts.IsEmpty())
	
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));

	const FVector HandLocationPosOther = Xform.InverseTransformPosition(ToLocation);
	
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPosOther));
	
	MoveArmRef->bEnableArmConstraints = false;
}

void UActionArmHandHolding::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MoveArmRef));
	check(IsValid(AnimSubsystem));
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms, 
			ElapsedTime, 
			PlayRate);
	
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
}

void UActionArmHandHolding::OnEnd()
{
	Super::OnEnd();
	
	check(IsValid(MoveArmRef));
	
	MoveArmRef->bEnableArmConstraints = true;
}