// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmFromHolster.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmFromHolster::UActionArmFromHolster(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmFromHolster::Init(UMoveArm* InMoveArm)
{
	MoveArmRef = InMoveArm;
}

void UActionArmFromHolster::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmFromHolster::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionArmFromHolster::IsDone_Implementation() const
{
	return false;
}
