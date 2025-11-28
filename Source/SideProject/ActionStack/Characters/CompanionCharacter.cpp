// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionCharacter.h"

#include "CompanionPawn.h"
#include "ActionStack/Actions/ActionMoveTo.h"

void UCompanionCharacter::Initialize(APawn* InActorOwner)
{
	ActorOwner = InActorOwner;
}
void UCompanionCharacter::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!bFirstTime || !ActorOwner) return;

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
