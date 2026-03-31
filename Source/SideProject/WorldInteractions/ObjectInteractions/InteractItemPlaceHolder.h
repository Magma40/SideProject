// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractItemPlaceHolder.generated.h"

class ABaseInteractableCharacter;
class AInteractableItemBase;
class UActionArmHandToLocation;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractItemPlaceHolder : public UInteractionBehaviour
{
	GENERATED_BODY()

protected:
	UInteractItemPlaceHolder();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	
private:
	AInteractableItemBase* PlacedItem = nullptr;
	
public:
	UPROPERTY(EditAnywhere, Category="ItemPlaceHolderReferences") AInteractableItemBase* ItemNeeded = nullptr;
};

