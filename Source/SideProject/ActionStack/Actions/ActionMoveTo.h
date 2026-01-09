// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "ActionStack/ActionStack.h"
#include "Navigation/PathFollowingComponent.h"
#include "ActionMoveTo.generated.h"

class ABaseInteractableCharacter;
class AMovingModel;

/**
 * Moves a CompanionCharacter to a target location using AIController, I had no idea this is how you write a summary in unreal
 */
UCLASS()
class SIDEPROJECT_API UActionMoveTo : public UAction
{
	GENERATED_BODY()

public:
	UActionMoveTo(const FObjectInitializer& ObjectInitializer);
	void Init(ABaseInteractableCharacter* InCompanionCharacter, FVector InTargetLocation, bool InNeedIndicator);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnEnd_Implementation() override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	ABaseInteractableCharacter* CompanionCharacter = nullptr;
	FVector TargetLocation = FVector::ZeroVector;
	float MinDistance = 50.0f;
	AActor* IndicatorActor = nullptr;
	bool bNeedIndicator = true;
	
	bool bMoveCompleted = false;
	 void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
