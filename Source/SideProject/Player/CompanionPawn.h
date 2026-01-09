// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player/BaseInteractableCharacter.h"
#include "CompanionPawn.generated.h"

UCLASS()
class SIDEPROJECT_API ACompanionPawn : public ABaseInteractableCharacter
{
	GENERATED_BODY()

public:
	ACompanionPawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
