// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionContinuousMoveTo.generated.h"

class ABaseInteractableCharacter;
class AAIController;

/**
 * Moves a CompanionCharacter to a target location using AIController, I had no idea this is how you write a summary in unreal
 */
UCLASS()
class SIDEPROJECT_API UActionContinuousMoveTo : public UAction
{
	GENERATED_BODY()

public:
	static UActionContinuousMoveTo* Create(UObject* Outer, ABaseInteractableCharacter* InCompanionCharacter, ABaseInteractableCharacter* InOtherCharacter);
	bool IsDone_Implementation() const override;
	void OnEnd_Implementation() override;
	void OnUpdate_Implementation(float DeltaTime) override;
	void OnBegin_Implementation(bool bFirstTime) override;

	void StartMove() const;

private:
	ABaseInteractableCharacter* CompanionCharacter = nullptr;
	ABaseInteractableCharacter* OtherCharacter = nullptr;

	TWeakObjectPtr<AAIController> CachedController;
	FTimerHandle FollowTimer;
	float FollowDistance = 1.0f;
	float CheckInterval = 0.1f;
};
