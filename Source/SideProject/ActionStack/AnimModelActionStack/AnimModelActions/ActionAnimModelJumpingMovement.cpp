// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelJumpingMovement.h"

UActionAnimModelJumpingMovement* UActionAnimModelJumpingMovement::Create(UObject* Outer)
{
	UActionAnimModelJumpingMovement* Action = NewObject<UActionAnimModelJumpingMovement>(Outer);
	return Action;
}

void UActionAnimModelJumpingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelJumpingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionAnimModelJumpingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
