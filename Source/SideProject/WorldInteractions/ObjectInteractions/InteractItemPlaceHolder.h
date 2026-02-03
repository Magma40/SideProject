// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractItemPlaceHolder.generated.h"

class ABaseInteractableCharacter;
class AInteractableItemBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractItemPlaceHolder : public UInteractionBehaviour
{
	GENERATED_BODY()

protected:
	UInteractItemPlaceHolder();
	~UInteractItemPlaceHolder();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable) void Interact_Implementation() override;
	bool CanInteract_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	float GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	
private:
	ABaseInteractableCharacter* InteractingCharacter = nullptr;
	bool Interacting = false;
	AInteractableItemBase* PlacedItem = nullptr;
	
public:
	UPROPERTY(EditAnywhere, Category="ItemPlaceHolderReferences") AActor* ToInteractActor = nullptr;
	UPROPERTY(EditAnywhere, Category="ItemPlaceHolderReferences") AInteractableItemBase* ItemNeeded = nullptr;
	UPROPERTY(EditAnywhere, Category="ItemPlaceHolderReferences") bool bOneUsePlacement = false;
};

