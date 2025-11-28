// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MainCharacter.generated.h"

class AMovingModel;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
UCLASS(abstract)
class AMainCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected: //Functions
	AMainCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Movement
	void Move(const FInputActionValue& Value);
	void ContinuousFalling(float Right, float Forward);

	//PickUpAndDrop
	UFUNCTION(BlueprintCallable) bool AbleToPickUpItem(); 						//Should be able to pick up item/ press E?
	UFUNCTION(BlueprintCallable) bool TryToPickUpItem(); 						//Should Drop item
	UFUNCTION(BlueprintCallable) bool TryToInsertItemIntoSocket(); 		//Should Pick up item
	UFUNCTION(BlueprintCallable) void DropItemInHand();							//Should Place down item into Socket
	UFUNCTION(BlueprintCallable) void PickUpItemToHand();
	UFUNCTION(BlueprintCallable) void InsertItemIntoSocket();
	UFUNCTION(BlueprintCallable) AActor* GetClosestActorItem(AActor* InActor); 	//Should Place down item into Socket
	UFUNCTION(BlueprintCallable) USceneComponent* GetClosestComponentItem(USceneComponent* InComponent); 	//Should Place down item into Socket
	static UPrimitiveComponent* GetPrimitiveFromActor(AActor* Actor);
	static void DisablePhysicsRecursively(USceneComponent* Comp);
	static void EnablePhysicsRecursively(USceneComponent* Comp);

public:
	FVector GetActionLocation() const { return NewActionLocation; }
	void SetActionLocation(const FVector& InNewActionLocation) { NewActionLocation = InNewActionLocation; }

public:
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoMove(float Right, float Forward);
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpStart() { Jump(); }
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpEnd() { StopJumping(); }

protected: //Variables
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MouseLookAction;

	UPROPERTY() ACameraActor* TheMainCameraInScene = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") bool Sprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float MaxSprintSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float WalkSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") bool EnabledInput = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float JumpSpeedForceChange = 100.0f;
	
	float LastOldForwardValue = 0.0f;
	float LastOldRightValue = 0.0f;
	float DefaultMaxSprintSpeed = 1.0f;

	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") TSubclassOf<UObject> ItemInHand;
	UPROPERTY(EditAnywhere, Category="PickUpAndDrop") float MinDistanceToPickUpItem = 200.0f;
	AMovingModel* MovingModel = nullptr;
	float LastKnownClosestDistance = 0.0f;
	AActor* ClosestActor = nullptr;
	USceneComponent* ClosestComponent = nullptr;

	FVector NewActionLocation = FVector::ZeroVector;
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

