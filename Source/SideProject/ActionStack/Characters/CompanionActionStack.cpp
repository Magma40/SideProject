// Fill out your copyright notice in the Description page of Project Settings.

#include "CompanionActionStack.h"
#include "Player/BaseInteractableCharacter.h"

#include "ActionStack/Actions/ActionMoveTo.h"
#include "ActionStack/Actions/ActionPickUpItem.h"
#include "ActionStack/Actions/ActionContinuousMoveTo.h"

UCompanionActionStack::UCompanionActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UCompanionActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
	
	Init();
}

void UCompanionActionStack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCompanionActionStack::Init()
{
	OwnerCharacter = Cast<ABaseInteractableCharacter>(GetOwner());
}

void UCompanionActionStack::OnAddActionMoveTo(FVector InMoveToLocation, bool InNeedIndicator)
{
	if (!OwnerCharacter) return;
	ActionMoveToRef = NewObject<UActionMoveTo>(this);
	ActionMoveToRef->Init(OwnerCharacter, InMoveToLocation, InNeedIndicator);
	this->PushAction(ActionMoveToRef);
}

void UCompanionActionStack::OnAddActionPickUp()
{
	if (!OwnerCharacter) return;
	ActionPickUpItemRef = NewObject<UActionPickUpItem>(this);
	ActionPickUpItemRef->Init(OwnerCharacter);
	this->PushAction(ActionPickUpItemRef);
}

void UCompanionActionStack::OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer)
{
	if (!OwnerCharacter) return;
	ActionContinuousMoveToRef = NewObject<UActionContinuousMoveTo>(this);
	ActionContinuousMoveToRef->Init(OwnerCharacter, InPlayer);
	this->PushAction(ActionContinuousMoveToRef);
}
