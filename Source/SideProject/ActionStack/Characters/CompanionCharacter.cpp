// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionCharacter.h"

#include "CompanionPawn.h"
#include "ActionStack/Actions/ActionMoveTo.h"
#include "ActionStack/Actions/ActionPickUpItem.h"
#include "ActionStack/Actions/ActionContinuousMoveTo.h"

void UCompanionCharacter::Initialize(APawn* InActorOwner)
{
	ActorOwner = InActorOwner;
}
void UCompanionCharacter::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!bFirstTime) return;
}

void UCompanionCharacter::OnAddActionMoveTo()
{
	if (!ActorOwner) return;
	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(ActorOwner);
	if (IsValid(CompanionPawn))
	{
		ActionMoveToRef = NewObject<UActionMoveTo>(this);
		ActionMoveToRef->Init(this, CompanionPawn->GetMoveToLocation(), CompanionPawn->MovingModel);
		UActionStack* Stack = UActionStack::GetMain(this);
		if (Stack)
		{
			Stack->PushAction(ActionMoveToRef);
		}
	}
}
void UCompanionCharacter::OnAddActionPickUp()
{
	if (!ActorOwner) return;
	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(ActorOwner);
	if (IsValid(CompanionPawn))
	{
		ActionPickUpItemRef = NewObject<UActionPickUpItem>(this);
		UActionStack* Stack = UActionStack::GetMain(this);
		ActionPickUpItemRef->Init(this);
		if (Stack)
		{
			Stack->PushAction(ActionPickUpItemRef);
		}
	}
}

void UCompanionCharacter::OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer)
{
	if (!ActorOwner) return;
	ABaseInteractableCharacter* CompanionPawn = Cast<ABaseInteractableCharacter>(ActorOwner);
	if (IsValid(CompanionPawn))
	{
		ActionContinuousMoveToRef = NewObject<UActionContinuousMoveTo>(this);
		UActionStack* Stack = UActionStack::GetMain(this);
		ActionContinuousMoveToRef->Init(CompanionPawn, InPlayer);
		if (Stack)
		{
			Stack->PushAction(ActionContinuousMoveToRef);
		}
	}
}
