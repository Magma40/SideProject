// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmMovingMovement.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmMovingMovement* UActionArmMovingMovement::Create(UObject* Outer, UMoveArm* InMoveArm)
{
	UActionArmMovingMovement* Action = NewObject<UActionArmMovingMovement>(Outer);
	Action->MoveArmRef = InMoveArm;
	return Action;
}

void UActionArmMovingMovement::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
}

void UActionArmMovingMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	if (MoveArmRef->ShouldHoldUpHands()) return;
	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
}

bool UActionArmMovingMovement::IsDone_Implementation() const
{
	return false;
}
