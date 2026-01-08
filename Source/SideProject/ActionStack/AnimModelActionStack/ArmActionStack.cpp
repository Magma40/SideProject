// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ProcedualAnimation/MoveArm.h"
#include "ActionStack/Characters/CompanionPawn.h"

#include "ArmActions/ActionArmMovingMovement.h"
#include "ArmActions/ActionArmJumpingMovement.h"
#include "ArmActions/ActionArmToHolster.h"
#include "ArmActions/ActionArmWave.h"
#include "ArmActions/ActionArmPoint.h"
#include "ArmActions/ActionArmHandHolding.h"
#include "ProcedualAnimation/MovingModel.h"


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
	if (Contains(ArmsWave) || Contains(ArmsToHolster)) return;
	if (Contains(ArmsPoint))
	{
		FVector PointingLocationRef = ArmsPoint->PointingLocation;
		ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCompanion);
		if (IsValid(ArmsPoint->ActorPointedAt))
		{
			CompanionPawn->AddPickUpItem();
			CompanionPawn->AddNewMoveTo(PointingLocationRef);
		}
		else
		{
			CompanionPawn->AddNewMoveTo(PointingLocationRef);
		}
		return;
	}
	
	ArmsWave = NewObject<UActionArmWave>(this);
	ArmsWave->Init(ArmRef, OwnerCharacter);
	this->PushAction(ArmsWave);
	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCompanion);
	CompanionPawn->AddNewMoveTo(OwnerCharacter->GetActorLocation());
}

void UArmActionStack::AddPointingAction()
{
	if (Contains(ArmsToHolster)  || Contains(ArmsWave)) return;
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
	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCompanion);
	ArmsHandHolding = NewObject<UActionArmHandHolding>(this);
	ArmsHandHolding->Init(ArmRef, OtherCompanionArm, OwnerCharacter, CompanionPawn);
	this->PushAction(ArmsHandHolding);
}
