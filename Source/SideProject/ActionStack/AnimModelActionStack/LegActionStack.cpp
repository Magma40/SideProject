// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/AnimModelActionStack/LegActionStack.h"

#include "LegActions/ActionLegMovingMovement.h"
#include "LegActions/ActionLegJumpingMovement.h"


ULegActionStack::ULegActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void ULegActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}

void ULegActionStack::Init(const TArray<UMoveLeg*>& InLegsRef, ABaseInteractableCharacter* InOwnerCharacter)
{
	LegsRef = InLegsRef;
	OwnerCharacter = InOwnerCharacter;
}

void ULegActionStack::AddMovingAction()
{
	LegDefaultMovement = NewObject<UActionLegMovingMovement>(this);
	LegDefaultMovement->Init(LegsRef);
	this->PushAction(LegDefaultMovement);
}
void ULegActionStack::AddJumpingAction()
{
	LegJumpingMovement = NewObject<UActionLegJumpingMovement>(this);
	LegJumpingMovement->Init(LegsRef, OwnerCharacter);
	this->PushAction(LegJumpingMovement);
}