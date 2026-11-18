#include "ActionStack/AnimModelActionStack/LegActionStack.h"
#include "ArmActionStack.h"
#include "LegActions/ActionLegMovingMovement.h"
#include "LegActions/ActionLegJumpingMovement.h"

void ULegActionStack::BeginPlay()
{
	Super::BeginPlay();
	
	Main = this;
}

void ULegActionStack::Init(const TArray<UMoveLeg*>& InLegsRef, ABaseInteractableCharacter* InOwnerCharacter)
{
	LegsRef = InLegsRef;
	OwnerCharacter = InOwnerCharacter;
}

void ULegActionStack::AddMovingAction()
{
	UActionLegMovingMovement* DefaultMovingAction = UActionLegMovingMovement::Create(this, 
		LegsRef);
	
	this->PushAction(DefaultMovingAction);
}

void ULegActionStack::AddJumpingAction()
{
	UActionLegJumpingMovement* LegJumpingMovement = UActionLegJumpingMovement::Create(this, 
		LegsRef, 
		OwnerCharacter);
	
	this->PushAction(LegJumpingMovement);
}