// Fill out your copyright notice in the Description page of Project Settings.
#include "CompanionActionStack.h"
#include "Player/BaseInteractableCharacter.h"
#include "ActionStack/Actions/ActionMoveTo.h"
#include "ActionStack/Actions/ActionPickUpItem.h"
#include "ActionStack/Actions/ActionContinuousMoveTo.h"
#include "ActionStack/Actions/ActionTryToInteractWithWorld.h"

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

void UCompanionActionStack::OnAddActionMoveTo(const FVector& InMoveToLocation, bool InNeedIndicator)
{
	if (!OwnerCharacter) return;
	if (UActionContinuousMoveTo* ActionRef = GetAction<UActionContinuousMoveTo>())  this->RemoveAction(ActionRef);
	if (UActionMoveTo* ActionRef = GetAction<UActionMoveTo>())  this->RemoveAction(ActionRef);
	UActionMoveTo* ActionMoveTo = UActionMoveTo::Create(this, OwnerCharacter, InMoveToLocation, InNeedIndicator);
	this->PushAction(ActionMoveTo);
}

void UCompanionActionStack::OnAddActionPickUp()
{
	if (!OwnerCharacter) return;
	if (UActionPickUpItem* ActionRef = GetAction<UActionPickUpItem>())  this->RemoveAction(ActionRef);
	if (UActionTryToInteractWithWorld* ActionRef = GetAction<UActionTryToInteractWithWorld>())  this->RemoveAction(ActionRef);
	UActionPickUpItem* ActionPickUpItem = UActionPickUpItem::Create(this, OwnerCharacter);
	this->PushAction(ActionPickUpItem);
}

void UCompanionActionStack::OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer)
{
	if (!OwnerCharacter) return;
	if (UActionContinuousMoveTo* ActionRef = GetAction<UActionContinuousMoveTo>())  this->RemoveAction(ActionRef);
	if (UActionMoveTo* ActionRef = GetAction<UActionMoveTo>())  this->RemoveAction(ActionRef);
	UActionContinuousMoveTo* ActionContinuousMoveTo = UActionContinuousMoveTo::Create(this, OwnerCharacter, InPlayer);
	this->PushAction(ActionContinuousMoveTo);
}

void UCompanionActionStack::OnAddTryToInteractWithWorld()
{
	if (!OwnerCharacter) return;
	if (UActionContinuousMoveTo* ActionRef = GetAction<UActionContinuousMoveTo>())  this->RemoveAction(ActionRef);
	if (UActionPickUpItem* ActionRef = GetAction<UActionPickUpItem>())  this->RemoveAction(ActionRef);
	if (UActionTryToInteractWithWorld* ActionRef = GetAction<UActionTryToInteractWithWorld>())  this->RemoveAction(ActionRef);
	UActionTryToInteractWithWorld* ActionContinuousMoveTo = UActionTryToInteractWithWorld::Create(this, OwnerCharacter);
	this->PushAction(ActionContinuousMoveTo);
}
