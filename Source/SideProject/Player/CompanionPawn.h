// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/BaseInteractableCharacter.h"
#include "CompanionPawn.generated.h"

class UInteractionBehaviour;

UCLASS()
class SIDEPROJECT_API ACompanionPawn : public ABaseInteractableCharacter
{
	GENERATED_BODY()

public:
	ACompanionPawn();
	 void SetLastKnownInteractionBehaviour(UInteractionBehaviour* InLastKnownInteractionBehaviour) { LastKnownInteractionBehaviour = InLastKnownInteractionBehaviour; }
	UFUNCTION(BlueprintCallable) virtual void RespawnCharacter() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY() UInteractionBehaviour* LastKnownInteractionBehaviour = nullptr;
	UFUNCTION(BlueprintCallable) void RunLastKnowInteract() const;
};
