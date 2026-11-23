#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmWave.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"

void UActionArmWave::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(MoveArmRef));
	check(!MoveArmRef->ArmParts.IsEmpty())

	//Calculate each animation point
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	const FVector MidOffset = HandLocationPos + FVector(-30.f, -25.f, 125.f); // offset
	const FVector MidOffset2 = HandLocationPos + FVector(+20.f, -25.f, 125.f); // offset
	const FVector MidOffset3 = HandLocationPos + FVector(0.0f, 0.0f, 125.f); // offset

	//Add each animation point to construct desired animation
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, MidOffset));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, MidOffset2));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, MidOffset3));
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
	
	MoveArmRef->bEnableArmConstraints = false;
}

void UActionArmWave::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MoveArmRef));
	check(IsValid(AnimSubsystem));

	//Do smooth custom animation 
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms, 
			ElapsedTime, 
			PlayRate);
	
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
	
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
}

void UActionArmWave::OnEnd()
{
	Super::OnEnd();
	
	check(IsValid(MoveArmRef));
	
	MoveArmRef->bEnableArmConstraints = true;
}

bool UActionArmWave::IsDone() const
{
	return AnimTransforms.Num() <= 1;
}