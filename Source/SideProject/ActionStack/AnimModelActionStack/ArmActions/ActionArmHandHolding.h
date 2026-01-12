// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmHandHolding.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmHandHolding : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionArmHandHolding* Create(UObject* Outer, UMoveArm* InMoveArm, UMoveArm* InOtherMoveArm, ABaseInteractableCharacter* InCharacter, ABaseInteractableCharacter* InAICharacter);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	UMoveArm* MoveArmRef = nullptr;
	UMoveArm* MoveOtherArmRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	ABaseInteractableCharacter* AIOwnerCharacter = nullptr;
	TArray<FVector> AnimLocations;
	float ElapsedTime = 0.0f;
	float PlayRate = 0.1f;

public:
	FVector PointingLocation = FVector::ZeroVector;
	UActorComponent* ActorPointedAt = nullptr;
};
