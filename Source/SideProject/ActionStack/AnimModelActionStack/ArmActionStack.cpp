// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"

#include "AnimModelActionStack.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "ArmActions/ActionArmMovingMovement.h"
#include "ArmActions/ActionArmJumpingMovement.h"
#include "ArmActions/ActionArmToHolster.h"
#include "ArmActions/ActionArmWave.h"
#include "ArmActions/ActionArmPoint.h"
#include "ArmActions/ActionArmHandHolding.h"
#include "ArmActions/ActionArmHandToLocation.h"
#include "Player/BaseInteractableCharacter.h"
#include "Player/CompanionPawn.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"
#include "WorldInteractions/InteractionManager.h"
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"


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
	if (UActionArmWave* ActionRef = GetAction<UActionArmWave>())  this->RemoveAction(ActionRef);
	if (UActionArmPoint* ActionRef = GetAction<UActionArmPoint>()) this->RemoveAction(ActionRef);
	if (UActionArmToHolster* ActionRef = GetAction<UActionArmToHolster>())  this->RemoveAction(ActionRef);
	if (UActionArmHandToLocation* ActionRef = GetAction<UActionArmHandToLocation>()) this->RemoveAction(ActionRef);
	
	UActionArmJumpingMovement* ArmsJumpingMovement = UActionArmJumpingMovement::Create(this, ArmRef);
	this->PushAction(ArmsJumpingMovement);
}

void UArmActionStack::AddHolsterAction()
{
	if (IsValid(GetAction<UActionArmToHolster>()) || GetAction<UActionArmJumpingMovement>() || GetAction<UActionArmHandToLocation>()) return;
	if (UActionArmWave* ActionRef = GetAction<UActionArmWave>())  this->RemoveAction(ActionRef);
	if (UActionArmPoint* ActionRef = GetAction<UActionArmPoint>()) this->RemoveAction(ActionRef);

	UActionArmToHolster* ArmsToHolster = UActionArmToHolster::Create(this, ArmRef, BackHolsterRef, OwnerCharacter);
	this->PushAction(ArmsToHolster);
}

void UArmActionStack::AddWaveAction()
{
	if (GetAction<UActionArmJumpingMovement>() || IsValid(GetAction<UActionArmWave>()) || GetAction<UActionArmHandToLocation>() || OwnerCharacter->GetItemInHand() != nullptr) return;
	if (UActionArmToHolster* ActionRef = GetAction<UActionArmToHolster>()) this->RemoveAction(ActionRef);
	if (UActionArmHandHolding* ActionRef = OwnerCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_L()->GetAction<UActionArmHandHolding>()) OwnerCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_L()->RemoveAction(ActionRef);
	if (UActionArmHandHolding* ActionRef = OwnerCompanion->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->GetAction<UActionArmHandHolding>()) OwnerCompanion->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->RemoveAction(ActionRef);

	if (!IsValid(OwnerCompanion)) return;
	if (OwnerCharacter->GetItemInHand() != nullptr) return;
	OwnerCompanion->GetCompanionActionStack()->ClearAllActionsExceptIndex(0);
	if (UActionArmPoint* PointAction = GetAction<UActionArmPoint>())
	{
		const FVector PointingLocationRef = PointAction->GetPointingLocation();
		if (IsValid(PointAction->GetActorPointedAt()) && Cast<AInteractableItemBase>(PointAction->GetActorPointedAt()))
		{
			AInteractableItemBase* Interaction = Cast<AInteractableItemBase>(PointAction->GetActorPointedAt());
			OwnerCompanion->GetCompanionActionStack()->OnAddActionPickUp();
			OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(Interaction->GetActorLocation(), false);
		}
		else if (IsValid(PointAction->GetActorPointedAt()) && Cast<UInteractionBehaviour>( GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(PointAction->GetActorPointedAt())))
		{
			UInteractionBehaviour* Interaction = Cast<UInteractionBehaviour>( GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(PointAction->GetActorPointedAt()));
			OwnerCompanion->GetCompanionActionStack()->OnAddTryToInteractWithWorld();
			OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(Interaction->GetComponentLocation(), false);
			Cast<ACompanionPawn>(OwnerCompanion)->SetLastKnownInteractionBehaviour(Cast<UInteractionBehaviour>( GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(PointAction->GetActorPointedAt())));
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
	if (GetAction<UActionArmPoint>() || GetAction<UActionArmJumpingMovement>() || GetAction<UActionArmHandToLocation>() || OwnerCharacter->GetItemInHand() != nullptr) return;
	if (UActionArmToHolster* ActionRef = GetAction<UActionArmToHolster>())  this->RemoveAction(ActionRef);
	if (UActionArmWave* ActionRef = GetAction<UActionArmWave>()) this->RemoveAction(ActionRef);
	UActionArmPoint* ArmsPoint = UActionArmPoint::Create(this, ArmRef, OwnerCharacter, OwnerCompanion);
	this->PushAction(ArmsPoint);
}

void UArmActionStack::RemovePointingAction()
{
	if (UActionArmPoint* ActionRef = GetAction<UActionArmPoint>()) this->RemoveAction(ActionRef);
}

void UArmActionStack::AddHandHoldingAction(UMoveArm* OtherCompanionArm)
{
	if (GetAction<UActionArmJumpingMovement>()) return;
	UActionArmHandHolding* ArmsHandHolding = UActionArmHandHolding::Create(this, ArmRef, OtherCompanionArm, OwnerCharacter, OwnerCompanion);
	this->PushAction(ArmsHandHolding);
}

void UArmActionStack::AddHandToLocation( const FVector& InToLocation, bool bWantToPickUpItem)
{
	if (GetAction<UActionArmJumpingMovement>()) return;
	if (UActionArmToHolster* ActionRef = GetAction<UActionArmToHolster>())  this->RemoveAction(ActionRef);
	if (UActionArmWave* ActionRef = GetAction<UActionArmWave>()) this->RemoveAction(ActionRef);
	if (UActionArmPoint* ActionRef = GetAction<UActionArmPoint>()) this->RemoveAction(ActionRef);
	if (UActionArmHandToLocation* ActionRef = GetAction<UActionArmHandToLocation>())
	{
		this->RemoveAction(ActionRef);
		return;
	}
	UActionArmHandToLocation* HandToLocation = UActionArmHandToLocation::Create(this, OwnerCharacter, ArmRef, InToLocation, bWantToPickUpItem);
	this->PushAction(HandToLocation);
	
}
