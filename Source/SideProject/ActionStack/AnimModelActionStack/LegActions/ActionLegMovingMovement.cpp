// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/LegActions/ActionLegMovingMovement.h"

UActionLegMovingMovement::UActionLegMovingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionLegMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionLegMovingMovement::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionLegMovingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
