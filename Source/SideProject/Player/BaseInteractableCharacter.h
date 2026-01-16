// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseInteractableCharacter.generated.h"

class UCompanionActionStack;
class APlayerCamera;

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
	UFUNCTION(BlueprintCallable) virtual void TryToPickUpItem();   //Looks around to see if anything can be picked up
	UFUNCTION(BlueprintCallable) virtual void DropItemInHand();	 //Should drop item in hand infront of character
	UFUNCTION(BlueprintCallable) virtual void PickUpItemToHand(UObject* InSpecificPickUpItem); //Should pick up item into hand

	UFUNCTION(BlueprintCallable) void TryToInteractWithBackHolster(); //Checks if you can either add or remove item from holster
	UFUNCTION(BlueprintCallable) void AttachToBackHolster(); //Should attach the ItemInHand into the holster
	UFUNCTION(BlueprintCallable) void DeattachFromBackHolster(); //Should deattach the item in the holster into the hand
	virtual void Jump() override;

	UObject* GetItemInHand() const { return ItemInHand; }
	UObject* GetItemInBackHolster() const { return BackItemHolster; }
	AMovingModel* GetMovingModel() const { return MovingModel; }
	UCompanionActionStack* GetCompanionActionStack() const { return CompanionActionStack; }
	APlayerCamera* GetCameraActor() const  { return TheMainCameraInScene; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") bool Sprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float MaxSprintSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float WalkSpeed = 200.0f;

private:
	UObject* ClosestObject = nullptr;

	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") UObject* ItemInHand = nullptr;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") UObject* BackItemHolster = nullptr;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") float MinDistanceToPickUpItem = 60.0f;
	USceneComponent* BackHolster = nullptr;

	TArray<FTransform> AnimTransforms;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;

protected:
	UPROPERTY() APlayerCamera* TheMainCameraInScene = nullptr;
	AMovingModel* MovingModel = nullptr;
	UCompanionActionStack* CompanionActionStack = nullptr;
};
