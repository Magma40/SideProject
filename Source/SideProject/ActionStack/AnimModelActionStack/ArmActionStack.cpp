// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/AnimModelActionStack/ArmActionStack.h"

#include "ArmActions/ActionArmMovingMovement.h"
#include "ArmActions/ActionArmJumpingMovement.h"
#include "ArmActions/ActionArmToHolster.h"
#include "ArmActions/ActionArmFromHolster.h"
#include "ProcedualAnimation/MoveArm.h"

UArmActionStack::UArmActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UArmActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}

void UArmActionStack::Init(const TArray<UMoveArm*>& InArmsRef, ABaseInteractableCharacter* InOwnerCharacter)
{
	ArmsRef = InArmsRef;
	OwnerCharacter = InOwnerCharacter;
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
	ArmsFromHolster = NewObject<UActionArmFromHolster>(this);
	UMoveArm* RightArm = nullptr;
	for(UMoveArm* Arm : ArmsRef)
	{
		if (Arm->GetName().EndsWith(TEXT("_R")))
		{
			RightArm =  Arm;
		}
	}
	ArmsFromHolster->Init(RightArm);
	this->PushAction(ArmsFromHolster);
	
	ArmsToHolster = NewObject<UActionArmToHolster>(this);
	ArmsToHolster->Init(RightArm);
	this->PushAction(ArmsToHolster);
}
