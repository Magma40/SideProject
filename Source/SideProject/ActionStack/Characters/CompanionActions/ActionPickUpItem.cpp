#include "ActionStack/Characters/CompanionActions/ActionPickUpItem.h"
#include "Player/BaseInteractableCharacter.h"

void UActionPickUpItem::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(CompanionCharacter));
	
	CompanionCharacter->TryToPickUpItem();
	
	TriedToPickUpItem = true;
}

bool UActionPickUpItem::IsDone() const
{
	return TriedToPickUpItem;
}