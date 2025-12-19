// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"

UActionAnimModelMovingMovement::UActionAnimModelMovingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionAnimModelMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelMovingMovement::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionAnimModelMovingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
