#include "ActionTryToInteractWithWorld.h"
#include "Player/BaseInteractableCharacter.h"


UActionTryToInteractWithWorld* UActionTryToInteractWithWorld::Create(UObject* Outer, ABaseInteractableCharacter* InCompanionCharacter)
{
	UActionTryToInteractWithWorld* Action = NewObject<UActionTryToInteractWithWorld>(Outer);
	Action->CompanionCharacter = InCompanionCharacter;
	return Action;
}

void UActionTryToInteractWithWorld::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!CompanionCharacter) return;
	CompanionCharacter->TryToInteractWithWorld();
	TriedToInteractWithWorld = true;
}


void UActionTryToInteractWithWorld::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionTryToInteractWithWorld::IsDone_Implementation() const
{
	if (!CompanionCharacter) return true;
	return  TriedToInteractWithWorld;
}