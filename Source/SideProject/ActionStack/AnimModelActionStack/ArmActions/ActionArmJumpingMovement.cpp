// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmJumpingMovement.h"

UActionArmJumpingMovement::UActionArmJumpingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmJumpingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmJumpingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionArmJumpingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
