// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionLegJumpingMovement.generated.h"

class ABaseInteractableCharacter;
class UMoveLeg;

UCLASS()
class SIDEPROJECT_API UActionLegJumpingMovement : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionLegJumpingMovement* Create(UObject* Outer, const TArray<UMoveLeg*>& InMoveLegs, ABaseInteractableCharacter* InOwnerCharacter);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
	void OnEnd_Implementation() override;

private:
	TArray<UMoveLeg*> MoveLegsRef;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
};
