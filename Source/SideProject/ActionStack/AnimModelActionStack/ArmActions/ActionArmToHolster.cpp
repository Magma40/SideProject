// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmToHolster.h"

#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"

UActionArmToHolster::UActionArmToHolster(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void UActionArmToHolster::Init(UMoveArm* InMoveArm, USceneComponent* InBackHolster, ABaseInteractableCharacter* InCharacter)
{
	MoveArmRef = InMoveArm;
	BackHolsterRef = InBackHolster;
	OwnerCharacter = InCharacter;
}

void UActionArmToHolster::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HolsterLocationPos = Xform.InverseTransformPosition(BackHolsterRef->GetComponentLocation());
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	FVector MidOffset = HandLocationPos + FVector(-10.f, -15.f, 75.f); // offset

	//To Holster
	AnimLocations.Insert(HandLocationPos, 0);
	AnimLocations.Insert(MidOffset, 1);
	AnimLocations.Insert(HolsterLocationPos, 2);

	//From Holster
	AnimLocations.Insert(MidOffset, 3);
	AnimLocations.Insert(HandLocationPos, 4);
	
	// UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();;
	// if (!AnimSubsystem)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UMoveArm - SetWaveAnim - AnimSubsystem was nullptr"));
	// 	return;
	// }
	// AnimLocations = AnimSubsystem->GenerateInBetweenAnimPositions(AnimLocations, 6);
}

//Discovering Tuple and Pairs is the best thing that has happened to me
void UActionArmToHolster::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}

	const int BeforeMoveNum = AnimLocations.Num();
	const TTuple<TTuple<TArray<FVector>, float>,FVector> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimLocations, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value;
	AnimLocations = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;

	if (BeforeMoveNum != AnimLocations.Num())
	{
		OwnerCharacter->TryToInteractWithBackHolster();
	}
}

bool UActionArmToHolster::IsDone_Implementation() const
{
	return AnimLocations.Num() <= 1;
}
