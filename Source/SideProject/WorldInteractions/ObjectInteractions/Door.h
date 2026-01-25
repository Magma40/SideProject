// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WorldInteractions/InteractionManager.h"
#include "Door.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UDoor : public UInteractionBehaviour
{
	GENERATED_BODY()

protected:
	UDoor();
	~UDoor();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool CanInteract_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	float GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;
	void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override;

	void Unlock();

private:
	ABaseInteractableCharacter* InteractingCharacter = nullptr;
	bool Interacting = false;

public:
	UPROPERTY(EditAnywhere, Category="DoorReferences") AActor* Lock = nullptr;
	UPROPERTY(EditAnywhere, Category="DoorReferences") TSubclassOf<UObject> ItemNeeded = nullptr;
};

