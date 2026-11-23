#include "AnimModelActionStack.h"
#include "ProcedualAnimation/MovingModel.h" 
#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"
#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelJumpingMovement.h"
#include "ActionStack/AnimModelActionStack/LegActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "Player/BaseInteractableCharacter.h"

void UAnimModelActionStack::PushDefaultActionMovement()
{
	check(IsValid(MovingModel));
	check(IsValid(LegsActionStack));
	check(!ArmsActionStack.IsEmpty());
	
	UActionAnimModelMovingMovement* DefaultMovement = 
		CreateAction<UActionAnimModelMovingMovement>(this, 
		MovingModel);
	
	PushAction(*DefaultMovement);

	LegsActionStack->AddMovingAction();
	ArmsActionStack[0]->AddMovingAction();
	ArmsActionStack[1]->AddMovingAction();
}

void UAnimModelActionStack::PushJumpingActionMovement()
{
	check(IsValid(OwnerCharacter));
	
	UActionAnimModelJumpingMovement* JumpingMovement = 
		CreateAction<UActionAnimModelJumpingMovement>(this, 
		OwnerCharacter);
	
	PushAction(*JumpingMovement);
	
	LegsActionStack->AddJumpingAction();
	ArmsActionStack[0]->AddJumpingAction();
	ArmsActionStack[1]->AddJumpingAction();
}

void UAnimModelActionStack::PushHolsterActionMovement() const
{
	check(!ArmsActionStack.IsEmpty());
	
	ArmsActionStack[1]->AddHolsterAction();
}

void UAnimModelActionStack::PushWaveActionMovement() const
{
	check(!ArmsActionStack.IsEmpty());
	
	ArmsActionStack[1]->AddWaveAction();
}

void UAnimModelActionStack::PushPointingActionMovement(const FVector& InPointingAtLocation) const
{
	check(!ArmsActionStack.IsEmpty());
	
	ArmsActionStack[1]->AddPointingAction(InPointingAtLocation);
}

void UAnimModelActionStack::PushHandHoldingActionMovement_R(const FVector& InOtherHandLocation) const
{
	check(!ArmsActionStack.IsEmpty());
	
	ArmsActionStack[1]->AddHandHoldingAction(InOtherHandLocation);
}

void UAnimModelActionStack::PushHandHoldingActionMovement_L(const FVector& InOtherHandLocation) const
{
	check(!ArmsActionStack.IsEmpty());
	
	ArmsActionStack[0]->AddHandHoldingAction(InOtherHandLocation);
}
