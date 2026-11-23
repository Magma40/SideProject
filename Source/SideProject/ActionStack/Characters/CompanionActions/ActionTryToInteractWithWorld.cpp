#include "ActionTryToInteractWithWorld.h"
#include "Player/BaseInteractableCharacter.h"

void UActionTryToInteractWithWorld::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(CompanionCharacter));
	
	CompanionCharacter->TryToInteractWithWorld();
	
	TriedToInteractWithWorld = true;
}

bool UActionTryToInteractWithWorld::IsDone() const
{
	return TriedToInteractWithWorld;
}