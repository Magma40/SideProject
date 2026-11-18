#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionMovingMultiple.generated.h"

class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API AInteractionMovingMultiple : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
	AInteractionMovingMultiple();
	
	//FUNCTIONS:
public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable) 
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	TObjectPtr<AActor> ValidActorPatternCallInteract = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	TObjectPtr<AActor> InvalidActorPatternCallInteract = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") 
	TArray<TObjectPtr<AActor>> ActorsToCallInteract;

private:
	UPROPERTY() 
	TArray<TScriptInterface<IInteractionInterface>> InteractionsToCallInteract;
	
	UPROPERTY() 
	TScriptInterface<IInteractionInterface> ValidInteractionPatternCallInteract = nullptr;
	
	UPROPERTY() 
	TScriptInterface<IInteractionInterface> InvalidInteractionPatternCallInteract = nullptr;
	
	UPROPERTY() 
	TArray<bool> CurrentPattern;
};
