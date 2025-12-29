// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseInteractableCharacter.generated.h"

struct FPickupCandidate
{
	UObject* Object = nullptr;           // Could be actor OR component
	float Distance = FLT_MAX;
};

class AMovingModel;

UCLASS()
class SIDEPROJECT_API ABaseInteractableCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	ABaseInteractableCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	//PickUpAndDrop
	UFUNCTION(BlueprintCallable) virtual bool ActionPickUpItem();   //Try to pick up items through ActionStack
	UFUNCTION(BlueprintCallable) virtual bool AbleToPickUpItem();  //Should try to pick up item / press E
	UFUNCTION(BlueprintCallable) virtual bool TryToPickUpItem();   //Looks around to see if anything can be picked up
	UFUNCTION(BlueprintCallable) virtual void DropItemInHand();	 //Should drop item in hand infront of character
	UFUNCTION(BlueprintCallable) virtual void PickUpItemToHand(UObject* inSpecificPickUpItem); //Should pick up item into hand

	UFUNCTION(BlueprintCallable) void TryToInteractWithBackHolster(); //Checks if you can either add or remove item from holster
	UFUNCTION(BlueprintCallable) void AttachToBackHolster(); //Should attach the ItemInHand into the holster
	UFUNCTION(BlueprintCallable) void DeattachFromBackHolster(); //Should deattach the item in the holster into the hand

	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") UObject* ItemInHand;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") UObject* BackItemHolster;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") float MinDistanceToPickUpItem = 200.0f;
	AMovingModel* MovingModel = nullptr;
	USceneComponent* BackHolster = nullptr;

private:
	UObject* ClosestObject = nullptr;
};
