// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookerSettings.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

class UActionArmMovingMovement;
class UActionArmJumpingMovement;
class UActionArmToHolster;
class UActionArmWave;
class UActionArmPoint;
class UActionArmHandHolding;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
protected:
	UArmActionStack();
	virtual void BeginPlay() override;

public:
	void Init(UMoveArm*  InArmsRef, ABaseInteractableCharacter* InOwnerCharacter,  ABaseInteractableCharacter* InOwnerCompanion, USceneComponent* InBackHolster);
	void AddMovingAction();
	void AddJumpingAction();
	void AddHolsterAction();
	void AddWaveAction();
	void AddPointingAction();
	void AddHandHoldingAction(UMoveArm* OtherCompanionArm);

	UMoveArm* GetArm() const { return ArmRef; }
	UActionArmHandHolding* GetHandHoldingAction() const { return ArmsHandHolding; }

private:
	UMoveArm* ArmRef;
	UPROPERTY() USceneComponent* BackHolsterRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	ABaseInteractableCharacter* OwnerCompanion = nullptr;
	
	UPROPERTY() UActionArmMovingMovement* ArmsDefaultMovement = nullptr;
	UPROPERTY() UActionArmJumpingMovement* ArmsJumpingMovement = nullptr;
	UPROPERTY() UActionArmToHolster* ArmsToHolster = nullptr;
	UPROPERTY() UActionArmWave* ArmsWave = nullptr;
	UPROPERTY() UActionArmPoint* ArmsPoint = nullptr;
	UPROPERTY() UActionArmHandHolding* ArmsHandHolding = nullptr;
};
