// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelIdleMovement.h"

UActionAnimModelIdleMovement::UActionAnimModelIdleMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionAnimModelIdleMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelIdleMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionAnimModelIdleMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
