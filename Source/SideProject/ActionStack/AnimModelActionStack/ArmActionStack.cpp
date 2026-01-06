// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ProcedualAnimation/MoveArm.h"
#include "ActionStack/Characters/CompanionPawn.h"

#include "ArmActions/ActionArmMovingMovement.h"
#include "ArmActions/ActionArmJumpingMovement.h"
#include "ArmActions/ActionArmToHolster.h"
#include "ArmActions/ActionArmWave.h"
#include "ArmActions/ActionArmPoint.h"


UArmActionStack::UArmActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UArmActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}

void UArmActionStack::Init(const TArray<UMoveArm*>& InArmsRef, ABaseInteractableCharacter* InOwnerCharacter, USceneComponent* InBackHolster, ACompanionPawn* InAICompanion)
{
	ArmsRef = InArmsRef;
	OwnerCharacter = InOwnerCharacter;
	BackHolsterRef = InBackHolster;
	AICompanion = InAICompanion;
}

void UArmActionStack::AddMovingAction()
{
	ArmsDefaultMovement = NewObject<UActionArmMovingMovement>(this);
	ArmsDefaultMovement->Init(ArmsRef);
	this->PushAction(ArmsDefaultMovement);
}
void UArmActionStack::AddJumpingAction()
{
	ArmsJumpingMovement = NewObject<UActionArmJumpingMovement>(this);
	ArmsJumpingMovement->Init(ArmsRef);
	this->PushAction(ArmsJumpingMovement);
}

void UArmActionStack::AddHolsterAction()
{
	if (Contains(ArmsToHolster)  || Contains(ArmsWave) || Contains(ArmsPoint)) return;
	UMoveArm* RightArm = nullptr;
	for(UMoveArm* Arm : ArmsRef)
	{
		if (Arm->GetName().EndsWith(TEXT("_R")))
		{
			RightArm =  Arm;
		}
	}
	
	ArmsToHolster = NewObject<UActionArmToHolster>(this);
	ArmsToHolster->Init(RightArm, BackHolsterRef, OwnerCharacter);
	this->PushAction(ArmsToHolster);
}

void UArmActionStack::AddWaveAction()
{
	if (Contains(ArmsWave) || Contains(ArmsToHolster)) return;
	if (Contains(ArmsPoint))
	{
		FVector PointingLocationRef = ArmsPoint->PointingLocation;
		if (IsValid(ArmsPoint->ActorPointedAt))
		{
			AICompanion->AddPickUpItem();
			AICompanion->AddNewMoveTo(PointingLocationRef);
		}
		else
		{
			AICompanion->AddNewMoveTo(PointingLocationRef);
		}
		return;
	}
	
	UMoveArm* RightArm = nullptr;
	for(UMoveArm* Arm : ArmsRef)
	{
		if (Arm->GetName().EndsWith(TEXT("_R")))
		{
			RightArm =  Arm;
		}
	}
	
	ArmsWave = NewObject<UActionArmWave>(this);
	ArmsWave->Init(RightArm, OwnerCharacter);
	this->PushAction(ArmsWave);

	AICompanion->AddNewMoveTo(OwnerCharacter->GetActorLocation());
}

void UArmActionStack::AddPointingAction()
{
	if (Contains(ArmsToHolster)  || Contains(ArmsWave)) return;
	if (Contains(ArmsPoint))
	{
		this->RemoveAction(ArmsPoint);
		return;
	}
	UMoveArm* RightArm = nullptr;
	for(UMoveArm* Arm : ArmsRef)
	{
		if (Arm->GetName().EndsWith(TEXT("_R")))
		{
			RightArm =  Arm;
		}
	}
	ArmsPoint = NewObject<UActionArmPoint>(this);
	ArmsPoint->Init(RightArm, OwnerCharacter);
	this->PushAction(ArmsPoint);
}
