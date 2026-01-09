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
	ArmsDefaultMovement = NewObject<UActionArmMovingMovement>(this);
	ArmsDefaultMovement->Init(ArmRef);
	this->PushAction(ArmsDefaultMovement);
}
void UArmActionStack::AddJumpingAction()
{
	ArmsJumpingMovement = NewObject<UActionArmJumpingMovement>(this);
	ArmsJumpingMovement->Init(ArmRef);
	this->PushAction(ArmsJumpingMovement);
}

void UArmActionStack::AddHolsterAction()
{
	if (Contains(ArmsToHolster)  || Contains(ArmsWave) || Contains(ArmsPoint)) return;
	
	ArmsToHolster = NewObject<UActionArmToHolster>(this);
	ArmsToHolster->Init(ArmRef, BackHolsterRef, OwnerCharacter);
	this->PushAction(ArmsToHolster);
}

void UArmActionStack::AddWaveAction()
{
	if (Contains(ArmsWave) || Contains(ArmsToHolster) || OwnerCharacter->ItemInHand != nullptr) return;
	if (Contains(ArmsPoint))
	{
		FVector PointingLocationRef = ArmsPoint->PointingLocation;
		if (IsValid(ArmsPoint->ActorPointedAt))
		{
			OwnerCompanion->CompanionActionStack->OnAddActionPickUp();
			OwnerCompanion->CompanionActionStack->OnAddActionMoveTo(PointingLocationRef, false);
		}
		else
		{
			OwnerCompanion->CompanionActionStack->OnAddActionMoveTo(PointingLocationRef, true);
		}
		return;
	}
	
	ArmsWave = NewObject<UActionArmWave>(this);
	ArmsWave->Init(ArmRef, OwnerCharacter);
	this->PushAction(ArmsWave);
	OwnerCompanion->CompanionActionStack->OnAddActionMoveTo(OwnerCharacter->GetActorLocation(), false);
}

void UArmActionStack::AddPointingAction()
{
	if (Contains(ArmsToHolster)  || Contains(ArmsWave) || OwnerCharacter->ItemInHand != nullptr) return;
	if (Contains(ArmsPoint))
	{
		this->RemoveAction(ArmsPoint);
		return;
	}
	ArmsPoint = NewObject<UActionArmPoint>(this);
	ArmsPoint->Init(ArmRef, OwnerCharacter);
	this->PushAction(ArmsPoint);
}

void UArmActionStack::AddHandHoldingAction(UMoveArm* OtherCompanionArm)
{
	ArmsHandHolding = NewObject<UActionArmHandHolding>(this);
	ArmsHandHolding->Init(ArmRef, OtherCompanionArm, OwnerCharacter, OwnerCompanion);
	this->PushAction(ArmsHandHolding);
}
