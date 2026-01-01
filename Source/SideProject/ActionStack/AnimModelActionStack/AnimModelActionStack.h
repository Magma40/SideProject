// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "AnimModelActionStack.generated.h"

class ABaseInteractableCharacter;
class UArmActionStack;
class ULegActionStack;
class AMovingModel;

class UActionAnimModelMovingMovement;
class UActionAnimModelMovingMovement;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UAnimModelActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	UAnimModelActionStack();
	virtual void BeginPlay() override;
	void Init();
	
	AMovingModel* MovingModelRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	ULegActionStack* LegActionStackRef = nullptr;
	UArmActionStack* ArmActionStackRef = nullptr;

	UPROPERTY() UActionAnimModelMovingMovement* DefaultMovement = nullptr;
	UPROPERTY() UActionAnimModelMovingMovement* AnimModelJumpingMovement = nullptr;

public:
	void PushDefaultActionMovement();
	void PushJumpingActionMovement() const;
	void PushHolsteActionMovement() const;
};
