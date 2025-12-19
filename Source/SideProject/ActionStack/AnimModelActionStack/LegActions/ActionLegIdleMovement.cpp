// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/LegActions/ActionLegIdleMovement.h"

UActionLegIdleMovement::UActionLegIdleMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionLegIdleMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionLegIdleMovement::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionLegIdleMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
