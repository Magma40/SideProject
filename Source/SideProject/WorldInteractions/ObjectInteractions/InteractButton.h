#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractButton.generated.h"

class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UInteractButton : public UInteractionBehaviour
{
	GENERATED_BODY()

	UInteractButton();

	//FUNCTIONS:
public:
	virtual void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
};

