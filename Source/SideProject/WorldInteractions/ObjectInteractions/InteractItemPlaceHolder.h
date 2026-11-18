#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractItemPlaceHolder.generated.h"

class ABaseInteractableCharacter;
class AInteractableItemBase;
class UActionArmHandToLocation;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UInteractItemPlaceHolder : public UInteractionBehaviour
{
	GENERATED_BODY()
	
	UInteractItemPlaceHolder();

	//FUNCTIONS:
public:
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	virtual void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	
protected:
	virtual void TickComponent(float DeltaTime,
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	//FIELDS
public:
	UPROPERTY(EditAnywhere, Category= "ItemPlaceHolderReferences") 
	TObjectPtr<AInteractableItemBase> ItemNeeded = nullptr;
	
private:
	UPROPERTY()
	TObjectPtr<AInteractableItemBase> PlacedItem = nullptr;
};

