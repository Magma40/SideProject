// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "ArmActions/ActionArmMovingMovement.h"
#include "ArmActions/ActionArmJumpingMovement.h"
#include "ArmActions/ActionArmToHolster.h"
#include "ArmActions/ActionArmWave.h"
#include "ArmActions/ActionArmPoint.h"
#include "ArmActions/ActionArmHandHolding.h"
#include "Player/BaseInteractableCharacter.h"


UArmActionStack::UArmActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UArmActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}

void UArmActionStack::Init(UMoveArm* InArmRef, ABaseInteractableCharacter* InOwnerCharacter, ABaseInteractableCharacter* InOwnerCompanion, USceneComponent* InBackHolster)
{
	ArmRef = InArmRef;
	OwnerCharacter = InOwnerCharacter;
	BackHolsterRef = InBackHolster;
	OwnerCompanion = InOwnerCompanion;
}

void UArmActionStack::AddMovingAction()
{
	UActionArmMovingMovement* ArmsDefaultMovement = UActionArmMovingMovement::Create(this, ArmRef);
	this->PushAction(ArmsDefaultMovement);
}
void UArmActionStack::AddJumpingAction()
{
	UActionArmJumpingMovement* ArmsJumpingMovement = UActionArmJumpingMovement::Create(this, ArmRef);
	this->PushAction(ArmsJumpingMovement);
}

void UArmActionStack::AddHolsterAction()
{
	if (IsValid(GetAction<UActionArmToHolster>())) return;
	if (UActionArmWave* ActionRef = GetAction<UActionArmWave>())  this->RemoveAction(ActionRef);
	if (UActionArmPoint* ActionRef = GetAction<UActionArmPoint>()) this->RemoveAction(ActionRef);

	UActionArmToHolster* ArmsToHolster = UActionArmToHolster::Create(this, ArmRef, BackHolsterRef, OwnerCharacter);
	this->PushAction(ArmsToHolster);
}

void UArmActionStack::AddWaveAction()
{
	if (IsValid(GetAction<UActionArmWave>()) || OwnerCharacter->GetItemInHand() != nullptr) return;
	if (UActionArmToHolster* ActionRef = GetAction<UActionArmToHolster>()) this->RemoveAction(ActionRef);
	if (UActionArmHandHolding* ActionRef = GetAction<UActionArmHandHolding>()) this->RemoveAction(ActionRef);
	
	if (OwnerCharacter->GetItemInHand() != nullptr) return;
	OwnerCompanion->GetCompanionActionStack()->ClearAllActions();
	if (UActionArmPoint* PointAction = GetAction<UActionArmPoint>())
	{
		FVector PointingLocationRef = PointAction->GetPointingLocation();
		if (IsValid(PointAction->GetActorPointedAt()))
		{
			OwnerCompanion->GetCompanionActionStack()->OnAddActionPickUp();
			OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(PointingLocationRef, false);
		}
		else
		{
			OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(PointingLocationRef, true);
		}
		return;
	}

	UActionArmWave* ArmsWave = UActionArmWave::Create(this, ArmRef, OwnerCharacter);
	this->PushAction(ArmsWave);
	OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(OwnerCharacter->GetActorLocation(), false);
}

void UArmActionStack::AddPointingAction()
{
	if (OwnerCharacter->GetItemInHand() != nullptr) return;
	if (UActionArmToHolster* ActionRef = GetAction<UActionArmToHolster>())  this->RemoveAction(ActionRef);
	if (UActionArmWave* ActionRef = GetAction<UActionArmWave>()) this->RemoveAction(ActionRef);
	if (UActionArmPoint* ActionRef = GetAction<UActionArmPoint>())
	{
		this->RemoveAction(ActionRef);
		return;
	}
	UActionArmPoint* ArmsPoint = UActionArmPoint::Create(this, ArmRef, OwnerCharacter);
	this->PushAction(ArmsPoint);
}

void UArmActionStack::AddHandHoldingAction(UMoveArm* OtherCompanionArm)
{
	UActionArmHandHolding* ArmsHandHolding = UActionArmHandHolding::Create(this, ArmRef, OtherCompanionArm, OwnerCharacter, OwnerCompanion);
	this->PushAction(ArmsHandHolding);
}
