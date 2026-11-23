#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmToHolster.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"

void UActionArmToHolster::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(MoveArmRef));
	check(!MoveArmRef->ArmParts.IsEmpty())
	
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FTransform HandTransform =  FTransform(FRotator(0, 0, 0),  Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation()));
	const FTransform MidOffset =  FTransform(FRotator(0, 0, -90),  HandTransform.GetLocation() + FVector(-20.f, -15.f, 75.f));
	const FTransform HolsterTransform =  FTransform( FRotator(25, 90, -180),   Xform.InverseTransformPosition(BackHolsterRef->GetComponentLocation()));

	//To Holster
	AnimTransforms.Add(HandTransform);
	AnimTransforms.Add(MidOffset);
	AnimTransforms.Add(HolsterTransform);

	//From Holster
	AnimTransforms.Add(MidOffset);
	AnimTransforms.Add(HandTransform);
	
	MoveArmRef->bEnableArmConstraints = false;
}

//Discovering Tuple and Pairs is the best thing that has happened to me
void UActionArmToHolster::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MoveArmRef));
	check(IsValid(AnimSubsystem));
	check(IsValid(OwnerCharacter));

	const int BeforeMoveNum = AnimTransforms.Num();
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms,
			ElapsedTime, 
			PlayRate);
	
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
	
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
	
	if (BeforeMoveNum != AnimTransforms.Num() && AnimTransforms.Num() == 3)
	{
		OwnerCharacter->TryToInteractWithBackHolster();
	}
}

void UActionArmToHolster::OnEnd()
{
	Super::OnEnd();
	
	check(IsValid(MoveArmRef));
	
	MoveArmRef->bEnableArmConstraints = true;
}

bool UActionArmToHolster::IsDone() const
{
	return AnimTransforms.Num() <= 1;
}