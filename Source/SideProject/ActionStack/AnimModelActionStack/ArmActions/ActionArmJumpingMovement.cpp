#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmJumpingMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/BaseInteractableCharacter.h"

bool UActionArmJumpingMovement::IsDone() const
{
	check(IsValid(OwnerCharacter));
	check(IsValid(OwnerCharacter->GetMovementComponent()));
	
	return !OwnerCharacter->GetMovementComponent()->IsFalling();
}