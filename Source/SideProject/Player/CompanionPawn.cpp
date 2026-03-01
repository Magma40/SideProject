// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CompanionPawn.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandHolding.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"
#include "WorldInteractions/InteractionManager.h"

ACompanionPawn::ACompanionPawn() { PrimaryActorTick.bCanEverTick = true; }

void ACompanionPawn::RespawnCharacter()
{
	GetCompanionActionStack()->ClearAllActionsExceptIndex(0);
	if (const TScriptInterface<IActionInterface> PlayerHandHoldingAction = GetMovingModel()->GetAnimModelActionStack()->GetInterfacedAction(GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->GetAction<UActionArmHandHolding>()))
	{
		GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->RemoveAction(PlayerHandHoldingAction);
	}
	Super::RespawnCharacter();
}

void ACompanionPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Get CompanionActionStack
	CompanionActionStack = Cast<UCompanionActionStack>(GenericCalls::FindChildByExactName(this, TEXT("CompanionActionStack")));
	if (!CompanionActionStack)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - CompanionActionStack not found or wrong type"), *StaticClass()->GetName());
		return;
	}
}

//The Companion AI tries to run last known interaction
void ACompanionPawn::RunLastKnowInteract() const
{
	if (!IsValid(LastKnownInteractionBehaviour)) return;
	CompanionActionStack->ClearAllActionsExceptIndex(0);
	CompanionActionStack->OnAddTryToInteractWithWorld();
	CompanionActionStack->OnAddActionMoveTo(LastKnownInteractionBehaviour->GetOwner()->GetActorLocation(), false);
}
