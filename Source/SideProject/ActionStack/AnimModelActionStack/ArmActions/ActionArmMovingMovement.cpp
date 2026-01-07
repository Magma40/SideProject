// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmMovingMovement.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmMovingMovement::UActionArmMovingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmMovingMovement::Init(UMoveArm* InMoveArm)
{
	MoveArmsRef = InMoveArm;
}

void UActionArmMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmMovingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	if (MoveArmsRef->ShouldHoldUpHands()) return;
	MoveArmsRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmsRef->TargetPose.Rotation = FQuat::Identity;
}

bool UActionArmMovingMovement::IsDone_Implementation() const
{
	return false;
}
