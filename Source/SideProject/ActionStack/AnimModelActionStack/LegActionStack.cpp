#include "ActionStack/AnimModelActionStack/LegActionStack.h"
#include "ArmActionStack.h"
#include "LegActions/ActionLegMovingMovement.h"
#include "LegActions/ActionLegJumpingMovement.h"
#include "Player/BaseInteractableCharacter.h"

void ULegActionStack::AddMovingAction()
{
	check(!Legs.IsEmpty());
	
	UActionLegMovingMovement* DefaultMovingAction = 
		CreateAction<UActionLegMovingMovement>(this, 
		Legs);
	
	PushAction(*DefaultMovingAction);
}

void ULegActionStack::AddJumpingAction()
{
	check(IsValid(OwnerCharacter));
	check(!Legs.IsEmpty());
	
	UActionLegJumpingMovement* LegJumpingMovement = 
		CreateAction<UActionLegJumpingMovement>(this, 
		OwnerCharacter,
		Legs);
	
	PushAction(*LegJumpingMovement);
}