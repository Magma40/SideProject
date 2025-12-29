// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmIdleMovement.h"

UActionArmIdleMovement::UActionArmIdleMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmIdleMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmIdleMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionArmIdleMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
