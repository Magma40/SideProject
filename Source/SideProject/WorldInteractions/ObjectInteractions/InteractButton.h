// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractButton.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractButton : public UInteractionBehaviour
{
	GENERATED_BODY()

protected:
	UInteractButton();

public:
	void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
};

