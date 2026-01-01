// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;
class UActionArmMovingMovement;
class UActionArmJumpingMovement;
class UActionArmToHolster;
class UActionArmFromHolster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
protected:
	UArmActionStack();
	virtual void BeginPlay() override;

public:
	void Init(const TArray<UMoveArm*>& InArmsRef, ABaseInteractableCharacter* InOwnerCharacter);
	void AddMovingAction();
	void AddJumpingAction();
	void AddHolsterAction();

private:
	TArray<UMoveArm*> ArmsRef;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	UPROPERTY() UActionArmMovingMovement* ArmsDefaultMovement = nullptr;
	UPROPERTY() UActionArmJumpingMovement* ArmsJumpingMovement = nullptr;
	UPROPERTY() UActionArmToHolster* ArmsToHolster = nullptr;
	UPROPERTY() UActionArmFromHolster* ArmsFromHolster = nullptr;
};
