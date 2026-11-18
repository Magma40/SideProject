#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionMovingFence.generated.h"

class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API AInteractionMovingFence : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
	AInteractionMovingFence();
	
	//FUNCTIONS:
public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)  
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	TArray<UStaticMeshComponent*> virtual GetAllStaticMeshesInOwner_Implementation() override;
	
	//~ Begin Getters & Setters
	void ResetDoForever()
	{
		bDoForever = false;
	}
	//~ End Getters & Setters

	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	float PlayRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	FVector VectorMoveTo = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category="MovingFence") FComponentReference
	ComponentsToMoveRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	TObjectPtr<AActor> ActorToCallInteract = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	bool bDoForever = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	float InterwallTimer = 0.0f;
	
protected:
	UPROPERTY()
	TScriptInterface<IInteractionInterface> InteractionToCallInteract = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> InteractingCharacter = nullptr;
	
	UPROPERTY(BlueprintReadWrite) 
	bool bShouldOpen = false;
	
	UPROPERTY(BlueprintReadWrite) 
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	UPROPERTY()
	FTimerHandle MyTimerHandle;
};
