// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Actions/ActionPickUpItem.h"
#include "Player/BaseInteractableCharacter.h"

UActionPickUpItem::UActionPickUpItem(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionPickUpItem::Init(ABaseInteractableCharacter* InCompanionCharacter)
{
	CompanionCharacter = InCompanionCharacter;
}

void UActionPickUpItem::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!CompanionCharacter) return;
	if (CompanionCharacter->ActionPickUpItem())
	{
		TriedToPickUpItem = true;
	}
}


void UActionPickUpItem::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
}

bool UActionPickUpItem::IsDone_Implementation() const
{
	if (!CompanionCharacter) return true;
	return  TriedToPickUpItem;
}
