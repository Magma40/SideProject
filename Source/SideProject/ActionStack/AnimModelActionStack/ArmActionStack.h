// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;
class ACompanionPawn;

class UActionArmMovingMovement;
class UActionArmJumpingMovement;
class UActionArmToHolster;
class UActionArmWave;
class UActionArmPoint;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
protected:
	UArmActionStack();
	virtual void BeginPlay() override;

public:
	void Init(const TArray<UMoveArm*>& InArmsRef, ABaseInteractableCharacter* InOwnerCharacter, USceneComponent* InBackHolster, ACompanionPawn* InAICompanion);
	void AddMovingAction();
	void AddJumpingAction();
	void AddHolsterAction();
	void AddWaveAction();
	void AddPointingAction();

private:
	TArray<UMoveArm*> ArmsRef;
	UPROPERTY() USceneComponent* BackHolsterRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	ACompanionPawn* AICompanion = nullptr;
	
	UPROPERTY() UActionArmMovingMovement* ArmsDefaultMovement = nullptr;
	UPROPERTY() UActionArmJumpingMovement* ArmsJumpingMovement = nullptr;
	UPROPERTY() UActionArmToHolster* ArmsToHolster = nullptr;
	UPROPERTY() UActionArmWave* ArmsWave = nullptr;
	UPROPERTY() UActionArmPoint* ArmsPoint = nullptr;
};
