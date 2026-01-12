// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "LegActionStack.generated.h"

class UMoveLeg;
class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API ULegActionStack : public UActionStack
{
	GENERATED_BODY()

protected: // Functions
	ULegActionStack();
	virtual void BeginPlay() override;

public:
	void Init(const TArray<UMoveLeg*>& InLegsRef, ABaseInteractableCharacter* InOwnerCharacter);
	void AddMovingAction();
	void AddJumpingAction();
	

private: //Variables
	TArray<UMoveLeg*> LegsRef;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
};
