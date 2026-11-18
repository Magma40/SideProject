#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractWeightPlatter.generated.h"

class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UInteractWeightPlatter : public UInteractionBehaviour
{
	GENERATED_BODY()

	UInteractWeightPlatter();
};

