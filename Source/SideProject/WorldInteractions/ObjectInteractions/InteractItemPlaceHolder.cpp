// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ObjectInteractions/InteractItemPlaceHolder.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
//#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"

UInteractItemPlaceHolder::UInteractItemPlaceHolder()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
}

void UInteractItemPlaceHolder::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(PlacedItem))
	{
		Interact_Implementation(InteractingCharacter);
		return;
	}
	if (bInteracting)
	{
		UArmActionStack* ArmActionStack = InteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R();
		const TScriptInterface<IActionInterface> ActionInterfaceArmToLocation = ArmActionStack->GetInterfacedAction(ArmActionStack->GetAction<UActionArmHandToLocation>());
		if (!ArmActionStack->Contains(ActionInterfaceArmToLocation))
		{
			bInteracting = false;
			return;
		}
		
		if (!IsValid(InteractingCharacter->GetItemInHand()) || InteractingCharacter->GetItemInHand() != ItemNeeded) return;
		if (FVector::Distance(InteractingCharacter->GetItemInHand()->GetActorLocation(), this->GetComponentLocation()) < DistanceForInteraction / 10)
		{
			Interact_Implementation(InteractingCharacter);
		}
	}
}

void UInteractItemPlaceHolder::PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	Super::PerformInteraction_Implementation(InInteractingCharacter);
	if (IsValid(PlacedItem)) { PlacedItem = nullptr; return; }
	InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandToLocation(this->GetComponentLocation(), false);
}

void UInteractItemPlaceHolder::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	//If the desired item is in place, place down item and make so player cant interact with item anymore
	if (IsValid(InInteractingCharacter) && IsValid(InteractingCharacter->GetItemInHand()) && InteractingCharacter->GetItemInHand() == ItemNeeded)
	{
		InteractingCharacter->GetItemInHand()->AttachToComponent_Implementation(this);
		PlacedItem = InteractingCharacter->GetItemInHand();
		InteractingCharacter->ResetItemInHand();
		PlacedItem->SetPersistentlyFrozen(true);
	}
	Super::Interact_Implementation(InInteractingCharacter);
}


