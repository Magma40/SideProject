// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmJumpingMovement.h"

UActionArmJumpingMovement* UActionArmJumpingMovement::Create(UObject* Outer, UMoveArm* InMoveArm)
{
	UActionArmJumpingMovement* Action = NewObject<UActionArmJumpingMovement>(Outer);
	Action->MoveArmRef = InMoveArm;
	return Action;
}

void UActionArmJumpingMovement::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmJumpingMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionArmJumpingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
