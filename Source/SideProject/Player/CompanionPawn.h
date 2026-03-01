// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/BaseInteractableCharacter.h"
#include "CompanionPawn.generated.h"

class UCompanionWidgetIndicator;
class UInteractionBehaviour;

/**
 Class for the companion AI
*/

UCLASS()
class SIDEPROJECT_API ACompanionPawn : public ABaseInteractableCharacter
{
	GENERATED_BODY()

public:
	ACompanionPawn();
	 void SetLastKnownInteractionBehaviour(UInteractionBehaviour* InLastKnownInteractionBehaviour) { LastKnownInteractionBehaviour = InLastKnownInteractionBehaviour; }
	UFUNCTION(BlueprintCallable) virtual void RespawnCharacter() override;
	UCompanionWidgetIndicator* GetCompanionWidgetIndicator() const { return CompanionWidgetIndicator; }
	void SetCompanionWidgetIndicator(UCompanionWidgetIndicator* InNewWidgetIndicator) { CompanionWidgetIndicator = InNewWidgetIndicator; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY() UCompanionWidgetIndicator* CompanionWidgetIndicator = nullptr;
	UPROPERTY() UInteractionBehaviour* LastKnownInteractionBehaviour = nullptr;
	UFUNCTION(BlueprintCallable) void RunLastKnowInteract() const;
};
