// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseInteractableCharacter.generated.h"

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
	UFUNCTION(BlueprintCallable) virtual bool ActionPickUpItem();
	UFUNCTION(BlueprintCallable) virtual bool AbleToPickUpItem(); 																						//Should be able to pick up item/ press E?
	UFUNCTION(BlueprintCallable) virtual bool TryToPickUpItem(); 																						//Should Drop item
	UFUNCTION(BlueprintCallable) virtual void DropItemInHand();																							//Should Place down item into Socket
	UFUNCTION(BlueprintCallable) virtual void PickUpItemToHand();
	UFUNCTION(BlueprintCallable) AActor* GetClosestActorItem(AActor* InActor); 													//Should Place down item into Socket
	UFUNCTION(BlueprintCallable) USceneComponent* GetClosestComponentItem(USceneComponent* InComponent); 	//Should Place down item into Socket
	static UPrimitiveComponent* GetPrimitiveFromActor(AActor* Actor);
	static void DisablePhysicsRecursively(USceneComponent* Comp);
	static void EnablePhysicsRecursively(USceneComponent* Comp);

	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") TSubclassOf<UObject> ItemInHand;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") float MinDistanceToPickUpItem = 200.0f;
	AMovingModel* MovingModel = nullptr;
	float LastKnownClosestDistance = 0.0f;
	AActor* ClosestActor = nullptr;
	USceneComponent* ClosestComponent = nullptr;
};

static void GetAllComponentsWithTag( UWorld* World, FName Tag, TArray<USceneComponent*>& OutComponents)
{
	for (TObjectIterator<USceneComponent> It; It; ++It)
	{
		USceneComponent* Comp = *It;

		if (Comp->GetWorld() == World && Comp->ComponentTags.Contains(Tag))
		{
			OutComponents.Add(Comp);
		}
	}
}
