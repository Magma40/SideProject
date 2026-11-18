#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractDoor.generated.h"

class AInteractableItemBase;
class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UInteractDoor : public UInteractionBehaviour
{
	GENERATED_BODY()
	
	UInteractDoor();
	
	//FUNCTIONS:
public:
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	virtual void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	
	virtual TArray<UStaticMeshComponent*> GetAllStaticMeshesInOwner_Implementation() override;

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, Category="DoorReferences")
	TObjectPtr<AInteractableItemBase> ItemNeeded = nullptr;
	
	UPROPERTY(EditAnywhere, Category="DoorReferences") 
	TObjectPtr<AActor> Lock = nullptr;
	
	UPROPERTY(EditAnywhere, Category="DoorReferences") 
	FComponentReference PhysicsConstraintsRef;

private:
	float CachedDoorSwing = 0.0f;
};

