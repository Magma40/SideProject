// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CompanionActionStack.generated.h"

class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UCompanionActionStack : public UActionStack
{
	GENERATED_BODY()

	//FUNCTIONS
public:
	//Actions
	void OnAddActionMoveTo(const FVector& InMoveToLocation, bool InNeedIndicator);
	
	void OnAddActionPickUp();
	
	void OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer);
	
	void OnAddTryToInteractWithWorld();
	
	//~ Begin Getters & Setters
	FORCEINLINE ABaseInteractableCharacter* GetInteractableCharacter() const
	{
		return OwnerCharacter;
	}
	//~ End Getters & Setters
	
protected:
	virtual void BeginPlay() override;
	
	void Init();

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
};
