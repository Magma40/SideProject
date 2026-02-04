// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractDoor.generated.h"

class AInteractableItemBase;
class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractDoor : public UInteractionBehaviour
{
	GENERATED_BODY()

protected:
	UInteractDoor();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;

public:
	UPROPERTY(EditAnywhere, Category="DoorReferences") AInteractableItemBase* ItemNeeded = nullptr;
	UPROPERTY(EditAnywhere, Category="DoorReferences") AActor* Lock = nullptr;
	UPROPERTY(EditAnywhere, Category="DoorReferences") FComponentReference PhysicsConstraintsRef;
};

