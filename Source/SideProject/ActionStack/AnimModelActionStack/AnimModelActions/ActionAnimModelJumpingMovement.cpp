#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelJumpingMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/BaseInteractableCharacter.h"

bool UActionAnimModelJumpingMovement::IsDone() const
{
	check(IsValid(OwnerCharacter));
	check(IsValid(OwnerCharacter->GetMovementComponent()));
	
	return !OwnerCharacter->GetMovementComponent()->IsFalling();
}