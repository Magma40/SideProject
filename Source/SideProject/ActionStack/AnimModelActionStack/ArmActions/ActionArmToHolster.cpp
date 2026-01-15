// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmToHolster.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"

UActionArmToHolster* UActionArmToHolster::Create(UObject* Outer, UMoveArm* InMoveArm, USceneComponent* InBackHolster, ABaseInteractableCharacter* InCharacter)
{
	UActionArmToHolster* Action = NewObject<UActionArmToHolster>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->BackHolsterRef = InBackHolster;
	Action->OwnerCharacter = InCharacter;
	return Action;
}

void UActionArmToHolster::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
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

void UActionArmToHolster::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	MoveArmRef->bEnableArmConstraints = true;
}

//Discovering Tuple and Pairs is the best thing that has happened to me
void UActionArmToHolster::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}

	const int BeforeMoveNum = AnimTransforms.Num();
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
	
	if (BeforeMoveNum != AnimTransforms.Num() && AnimTransforms.Num() == 3)
	{
		OwnerCharacter->TryToInteractWithBackHolster();
	}
}

bool UActionArmToHolster::IsDone_Implementation() const
{
	return AnimTransforms.Num() <= 1;
}
