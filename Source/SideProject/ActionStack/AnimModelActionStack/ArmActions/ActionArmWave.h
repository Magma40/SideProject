// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmWave.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmWave : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionArmWave* Create(UObject* Outer, UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	UMoveArm* MoveArmRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	TArray<FVector> AnimLocations;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;
};
