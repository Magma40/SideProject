// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelJumpingMovement.h"

UActionAnimModelJumpingMovement::UActionAnimModelJumpingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionAnimModelJumpingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelJumpingMovement::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionAnimModelJumpingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
