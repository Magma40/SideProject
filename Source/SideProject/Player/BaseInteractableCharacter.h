#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseInteractableCharacter.generated.h"

class UCompanionActionStack;
class APlayerCamera;
class AInteractableItemBase;
class UMovingModel;

struct FPickupCandidate
{
	AInteractableItemBase* Object = nullptr;
	float Distance = FLT_MAX;
};

/**
 Base class for characters, used for picking items up and interacting with world
*/
UCLASS()
class SIDEPROJECT_API ABaseInteractableCharacter : public ACharacter
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	ABaseInteractableCharacter();
	
	//PickUpAndDrop
	//Looks around to see if anything can be picked up
	UFUNCTION(BlueprintCallable) 
	virtual void TryToPickUpItem();
	
	//Should drop item in hand in front of character
	UFUNCTION(BlueprintCallable) 
	virtual void DropItemInHand();
	
	//Should pick up item into hand
	UFUNCTION(BlueprintCallable) 
	virtual void PickUpItemToHand(AInteractableItemBase* InSpecificPickUpItem);

	//Checks if you can either add or remove item from holster
	UFUNCTION(BlueprintCallable) 
	void TryToInteractWithBackHolster();
	
	//Should attach the ItemInHand into the holster
	UFUNCTION(BlueprintCallable) 
	void AttachToBackHolster();
	
	//Should deattach the item in the holster into the hand
	UFUNCTION(BlueprintCallable) 
	void DeattachFromBackHolster();

	UFUNCTION(BlueprintCallable) 
	void TryToInteractWithWorld();
	
	virtual void Jump() override;
	
	virtual void RespawnCharacter();
	
	//~ Begin Getters & Setters
	UFUNCTION(BlueprintPure) 
	FORCEINLINE AInteractableItemBase* GetItemInHand() const
	{
		return ItemInHand;
	}
	
	FORCEINLINE void ResetItemInHand()
	{
		ItemInHand = nullptr; 
	}
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AInteractableItemBase* GetItemInBackHolster() const
	{
		return BackItemHolster;
	}
	
	UFUNCTION(BlueprintCallable) 
	FORCEINLINE UMovingModel* GetMovingModel() const
	{
		return MovingModel;
	}
	
	FORCEINLINE UCompanionActionStack* GetCompanionActionStack() const
	{
		return CompanionActionStack;
	}
	
	FORCEINLINE APlayerCamera* GetCameraActor() const
	{
		return TheMainCameraInScene;
	}
	
	FORCEINLINE bool IsCharacterInteracting() const
	{
		return InteractionEnabled;
	}
	
	FORCEINLINE void SetCharacterInteracting(const bool bNewCharacterInteracting)
	{
		InteractionEnabled = bNewCharacterInteracting;
	}
	
	FORCEINLINE void SetNewRespawnPoint(const FTransform& NewRespawnPoints)
	{
		RoomRespawnSpawnPoint = NewRespawnPoints;
	}
	//~ End Getters & Setters
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") 
	bool Sprinting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") 
	float MaxSprintSpeed = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") 
	float WalkSpeed = 200.0f;
	
protected:
	UPROPERTY() 
	TObjectPtr<APlayerCamera> TheMainCameraInScene = nullptr;
	
	UPROPERTY() 
	TObjectPtr<UMovingModel> MovingModel = nullptr;
	
	UPROPERTY() 
	TObjectPtr<UCompanionActionStack> CompanionActionStack = nullptr;
	
	UPROPERTY() 
	FTransform RoomRespawnSpawnPoint  = FTransform::Identity;

private:
	UPROPERTY()
	TObjectPtr<AInteractableItemBase> ClosestObject = nullptr;

	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") 
	TObjectPtr<AInteractableItemBase> ItemInHand = nullptr;
	
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop")
	TObjectPtr<AInteractableItemBase> BackItemHolster = nullptr;
	
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") 
	float MinDistanceToPickUpItem = 100.0f;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> BackHolster = nullptr;

	UPROPERTY() 
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 1.0f;

	bool InteractionEnabled = false;
};
