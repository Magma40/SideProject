// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/Characters/ActionCharacter.h"
#include "CompanionCharacter.generated.h"

class ABaseInteractableCharacter;
class UActionMoveTo;
class UActionPickUpItem;
class UActionContinuousMoveTo;

UCLASS()
class SIDEPROJECT_API UCompanionCharacter : public UActionCharacter
{
	GENERATED_BODY()

public:
	void Initialize(APawn* InActorOwner);
	APawn* GetOwnerCharacter() const { return ActorOwner; }
	void OnBegin_Implementation(bool bFirstTime) override;

	//Actions
	void OnAddActionMoveTo();
	void OnAddActionPickUp();
	void OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer);

	UActionMoveTo* GetMoveTo() const { return ActionMoveToRef; }
	UActionContinuousMoveTo* GetActionContinuousMoveTo() const { return ActionContinuousMoveToRef; }

private:

	UPROPERTY() UActionMoveTo* ActionMoveToRef;
	UPROPERTY() UActionPickUpItem* ActionPickUpItemRef;
	UPROPERTY() UActionContinuousMoveTo* ActionContinuousMoveToRef;
};

