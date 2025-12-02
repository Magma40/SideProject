// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Actions/ActionPickUpItem.h"

#include "Player/BaseInteractableCharacter.h"

UActionPickUpItem::UActionPickUpItem(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionPickUpItem::Init(UCompanionCharacter* InCompanionCharacter)
{
	CompanionCharacter = InCompanionCharacter;
}

void UActionPickUpItem::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!CompanionCharacter) return;
	
	APawn* Pawn = CompanionCharacter->GetOwnerCharacter();
	ABaseInteractableCharacter* InteractableCharacter = Cast<ABaseInteractableCharacter>(Pawn);
	if (!InteractableCharacter) return;

	if (InteractableCharacter->ActionPickUpItem())
	{
		TriedToPickUpItem = true;
	}
}


void UActionPickUpItem::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionPickUpItem::IsDone_Implementation() const
{
	if (!CompanionCharacter) return true;
	return  TriedToPickUpItem;
}
