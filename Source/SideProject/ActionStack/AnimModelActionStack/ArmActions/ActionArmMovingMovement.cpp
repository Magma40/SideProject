// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmMovingMovement.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmMovingMovement::UActionArmMovingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmMovingMovement::Init(TArray<UMoveArm*> InMoveArms)
{
	MoveArmsRef = InMoveArms;
}

void UActionArmMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmMovingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	for (UMoveArm* Arm : MoveArmsRef)
	{
		if (Arm->ShouldHoldUpHands()) continue;
		Arm->TargetPose.Position = FVector(0,25,  -50);
		Arm->TargetPose.Rotation = FQuat::Identity;
	}
}

bool UActionArmMovingMovement::IsDone_Implementation() const
{
	return false;
}
