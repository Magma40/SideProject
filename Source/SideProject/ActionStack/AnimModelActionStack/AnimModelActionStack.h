// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "AnimModelActionStack.generated.h"

class ABaseInteractableCharacter;
class UArmActionStack;
class ULegActionStack;
class AMovingModel;
class UActionArmHandHolding;
class UActionContinuousMoveTo;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UAnimModelActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	UAnimModelActionStack();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Init();
	
	
	AMovingModel* MovingModelRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	ABaseInteractableCharacter* CompanionOwnerCharacter = nullptr;
	ULegActionStack* LegActionStackRef = nullptr;
	UArmActionStack* ArmActionStack_RRef = nullptr;
	UArmActionStack* ArmActionStack_LRef = nullptr;

	float MinDistanceToHoldHands = 200.0f;

public:
	void PushDefaultActionMovement();
	void PushJumpingActionMovement();
	void PushHolsterActionMovement() const;
	void PushWaveActionMovement() const;
	void PushPointingActionMovement() const;
	void PushHandHoldingActionMovement();

	UArmActionStack* GetArmActionStack_R() { return ArmActionStack_RRef; }
	UArmActionStack* GetArmActionStack_L() { return ArmActionStack_LRef; }
	ULegActionStack* GetLegsActionStack() { return LegActionStackRef;}
};
