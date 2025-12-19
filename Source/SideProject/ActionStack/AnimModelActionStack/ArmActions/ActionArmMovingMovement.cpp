// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmMovingMovement.h"

UActionArmMovingMovement::UActionArmMovingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmMovingMovement::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionArmMovingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
