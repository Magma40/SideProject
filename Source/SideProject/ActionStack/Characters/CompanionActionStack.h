// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CompanionActionStack.generated.h"

class ABaseInteractableCharacter;
class UActionMoveTo;
class UActionPickUpItem;
class UActionContinuousMoveTo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UCompanionActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	UCompanionActionStack();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Init();
	
	ABaseInteractableCharacter* OwnerCharacter = nullptr;

public:
	//Actions
	void OnAddActionMoveTo(FVector InMoveToLocation, bool InNeedIndicator);
	void OnAddActionPickUp();
	void OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer);

	UActionMoveTo* GetActionMoveTo() const { return ActionMoveToRef; }
	UActionContinuousMoveTo* GetActionContinuousMoveTo() const { return ActionContinuousMoveToRef; }
	UActionPickUpItem* GetActionPickUp() const { return ActionPickUpItemRef; }

private:
	UPROPERTY() UActionMoveTo* ActionMoveToRef = nullptr;
	UPROPERTY() UActionPickUpItem* ActionPickUpItemRef = nullptr;
	UPROPERTY() UActionContinuousMoveTo* ActionContinuousMoveToRef = nullptr;
};
