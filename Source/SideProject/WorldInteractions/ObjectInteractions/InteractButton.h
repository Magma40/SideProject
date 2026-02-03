// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractButton.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractButton : public UInteractionBehaviour
{
	GENERATED_BODY()

protected:
	UInteractButton();
	~UInteractButton();
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable) void Interact_Implementation() override;
	bool CanInteract_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	float GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;

private:
	ABaseInteractableCharacter* InteractingCharacter = nullptr;
	bool Interacting = false;

public:
	UPROPERTY(EditAnywhere, Category="ButtonReferences") AActor* ToInteractActor = nullptr;
	UPROPERTY(EditAnywhere, Category="ButtonReferences") bool bOneUseButton = false;
};

