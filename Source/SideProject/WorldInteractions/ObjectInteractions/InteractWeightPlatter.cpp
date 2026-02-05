// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ObjectInteractions/InteractWeightPlatter.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"

UInteractWeightPlatter::UInteractWeightPlatter()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
}


