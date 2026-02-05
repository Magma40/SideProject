// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseInteractableCharacter.generated.h"

class UCompanionActionStack;
class APlayerCamera;
class AInteractableItemBase;
class AMovingModel;

struct FPickupCandidate
{
	AInteractableItemBase* Object = nullptr;
	float Distance = FLT_MAX;
};

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
	UFUNCTION(BlueprintCallable) virtual void TryToPickUpItem();   //Looks around to see if anything can be picked up
	UFUNCTION(BlueprintCallable) virtual void DropItemInHand();	 //Should drop item in hand infront of character
	UFUNCTION(BlueprintCallable) virtual void PickUpItemToHand(AInteractableItemBase* InSpecificPickUpItem); //Should pick up item into hand

	UFUNCTION(BlueprintCallable) void TryToInteractWithBackHolster(); //Checks if you can either add or remove item from holster
	UFUNCTION(BlueprintCallable) void AttachToBackHolster(); //Should attach the ItemInHand into the holster
	UFUNCTION(BlueprintCallable) void DeattachFromBackHolster(); //Should deattach the item in the holster into the hand

	UFUNCTION(BlueprintCallable) void TryToInteractWithWorld();
	
	virtual void Jump() override;
	
	UFUNCTION(BlueprintCallable) AInteractableItemBase* GetItemInHand() const { return ItemInHand; }
	void ResetItemInHand(){ ItemInHand = nullptr; }
	
	UFUNCTION(BlueprintCallable) AInteractableItemBase* GetItemInBackHolster() const { return BackItemHolster; }
	UFUNCTION(BlueprintCallable) AMovingModel* GetMovingModel() const { return MovingModel; }
	UCompanionActionStack* GetCompanionActionStack() const { return CompanionActionStack; }
	APlayerCamera* GetCameraActor() const  { return TheMainCameraInScene; }
	bool IsCharacterInteracting() const  { return InteractionEnabled; }
	void SetCharacterInteracting(const bool bNewCharacterInteracting){ InteractionEnabled = bNewCharacterInteracting; } ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") bool Sprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float MaxSprintSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float WalkSpeed = 200.0f;

private:
	AInteractableItemBase* ClosestObject = nullptr;

	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") AInteractableItemBase* ItemInHand = nullptr;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") AInteractableItemBase* BackItemHolster = nullptr;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") float MinDistanceToPickUpItem = 100.0f;
	USceneComponent* BackHolster = nullptr;

	TArray<FTransform> AnimTransforms;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;

	bool InteractionEnabled = false;

protected:
	UPROPERTY() APlayerCamera* TheMainCameraInScene = nullptr;
	AMovingModel* MovingModel = nullptr;
	UCompanionActionStack* CompanionActionStack = nullptr;
};
