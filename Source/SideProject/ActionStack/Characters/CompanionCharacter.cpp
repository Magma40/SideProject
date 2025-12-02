// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionCharacter.h"

#include "CompanionPawn.h"
#include "ActionStack/Actions/ActionMoveTo.h"
#include "ActionStack/Actions/ActionPickUpItem.h"
#include "Chaos/PBDSuspensionConstraintData.h"

void UCompanionCharacter::Initialize(APawn* InActorOwner)
{
	ActorOwner = InActorOwner;
}
void UCompanionCharacter::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!bFirstTime) return;
}

void UCompanionCharacter::OnAddActionMoveTo()
{
	if (!ActorOwner) return;
	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(ActorOwner);
	if (IsValid(CompanionPawn))
	{
		UActionMoveTo* MoveAction = NewObject<UActionMoveTo>(this);
		MoveAction->Init(this, CompanionPawn->GetMoveToLocation());
		UActionStack* Stack = UActionStack::GetMain(this);
		if (Stack)
		{
			Stack->PushAction(MoveAction);
		}
	}
}
void UCompanionCharacter::OnAddActionPickUp()
{
	if (!ActorOwner) return;
	ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(ActorOwner);
	if (IsValid(CompanionPawn))
	{
		UActionPickUpItem* PickUpAction = NewObject<UActionPickUpItem>(this);
		UActionStack* Stack = UActionStack::GetMain(this);
		PickUpAction->Init(this);
		if (Stack)
		{
			Stack->PushAction(PickUpAction);
		}
		
		CompanionPawn->SetMoveToLocation(FVector(360, -1240, 120));
		UActionMoveTo* MoveAction = NewObject<UActionMoveTo>(this);
		MoveAction->Init(this, CompanionPawn->GetMoveToLocation());
		if (Stack)
		{
			Stack->PushAction(MoveAction);
		}
	}
}
