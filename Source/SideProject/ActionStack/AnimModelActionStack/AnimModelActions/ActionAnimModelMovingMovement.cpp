// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"
#include "ProcedualAnimation/MovingModel.h"

UActionAnimModelMovingMovement::UActionAnimModelMovingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionAnimModelMovingMovement::Init(AMovingModel* InMovingModel)
{
	MovingModelRef = InMovingModel;
}

void UActionAnimModelMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelMovingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	MovingModelRef->UpdateHips();
}

bool UActionAnimModelMovingMovement::IsDone_Implementation() const
{
	return false;
}
