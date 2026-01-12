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

void UActionArmWave::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	FVector MidOffset = HandLocationPos + FVector(-30.f, -25.f, 125.f); // offset
	FVector MidOffset2 = HandLocationPos + FVector(+20.f, -25.f, 125.f); // offset
	FVector MidOffset3 = HandLocationPos + FVector(0.0f, 0.0f, 125.f); // offset
	
	AnimLocations.Insert(HandLocationPos, 0);
	AnimLocations.Insert(MidOffset, 1);
	AnimLocations.Insert(MidOffset2, 2);
	AnimLocations.Insert(MidOffset3, 3);
	AnimLocations.Insert(HandLocationPos, 4);
}

void UActionArmWave::OnUpdate_Implementation(float DeltaTime) 
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
	AnimLocations = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;
}

bool UActionArmWave::IsDone_Implementation() const
{
	return AnimLocations.Num() <= 1;
}
