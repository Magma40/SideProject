// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/Actions/ActionPickUpItem.h"
#include "Player/BaseInteractableCharacter.h"

UActionPickUpItem* UActionPickUpItem::Create(UObject* Outer, ABaseInteractableCharacter* InCompanionCharacter)
{
	UActionPickUpItem* Action = NewObject<UActionPickUpItem>(Outer);
	Action->CompanionCharacter = InCompanionCharacter;
	return Action;
}

void UActionPickUpItem::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!CompanionCharacter) return;
	CompanionCharacter->TryToPickUpItem();
	TriedToPickUpItem = true;
}


void UActionPickUpItem::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionPickUpItem::IsDone_Implementation() const
{
	if (!CompanionCharacter) return true;
	return  TriedToPickUpItem;
}
