#include "CompanionActionStack.h"
#include "Player/BaseInteractableCharacter.h"
#include "CompanionActions/ActionMoveTo.h"
#include "CompanionActions/ActionPickUpItem.h"
#include "CompanionActions/ActionContinuousMoveTo.h"
#include "ActionStack/Characters/CompanionActions/ActionTryToInteractWithWorld.h"

void UCompanionActionStack::OnAddActionMoveTo(const FVector& InMoveToLocation)
{
	check(IsValid(OwnerCharacter));
	
	UActionMoveTo* ActionMoveTo = 
		CreateAction<UActionMoveTo>(this, 
		OwnerCharacter, 
		InMoveToLocation, 
		MinDistanceForMoveTo);
	
	PushAction(*ActionMoveTo);
}

void UCompanionActionStack::OnAddActionPickUp()
{
	check(IsValid(OwnerCharacter));
	
	UActionPickUpItem* ActionPickUpItem = 
		CreateAction<UActionPickUpItem>(this, 
		OwnerCharacter);
	
	PushAction(*ActionPickUpItem);
}

void UCompanionActionStack::OnAddActionContinuousMove(const FVector& InMoveToLocation)
{
	check(IsValid(OwnerCharacter));
	
	UActionContinuousMoveTo* ActionContinuousMoveTo =
		CreateAction<UActionContinuousMoveTo>(this, 
		OwnerCharacter, 
		InMoveToLocation,
		MinDistanceForContinuousMoveTo);
	
	PushAction(*ActionContinuousMoveTo);
}

void UCompanionActionStack::OnAddTryToInteractWithWorld()
{
	check(IsValid(OwnerCharacter));
	
	UActionTryToInteractWithWorld* ActionContinuousMoveTo = 
		CreateAction<UActionTryToInteractWithWorld>(this, 
		OwnerCharacter);
	
	 PushAction(*ActionContinuousMoveTo);
}