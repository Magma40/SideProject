// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "AnimModelActionStack.generated.h"

class ABaseInteractableCharacter;
class UArmActionStack;
class ULegActionStack;
class AMovingModel;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UAnimModelActionStack : public UActionStack
{
	GENERATED_BODY()

private:
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

	void RemovePointingActionMovement() const;

	UFUNCTION(BlueprintCallable) UArmActionStack* GetArmActionStack_R() const { return ArmActionStack_RRef; }
	UFUNCTION(BlueprintCallable) UArmActionStack* GetArmActionStack_L() const { return ArmActionStack_LRef; }
	UFUNCTION(BlueprintCallable) ULegActionStack* GetLegsActionStack() const { return LegActionStackRef;}
};
