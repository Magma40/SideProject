// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/LegActions/ActionLegJumpingMovement.h"

UActionLegJumpingMovement::UActionLegJumpingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionLegJumpingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionLegJumpingMovement::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionLegJumpingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
