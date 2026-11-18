#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionMultiple.generated.h"

class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API AInteractionMultiple : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
	AInteractionMultiple();
	
	//FUNCTIONS:
public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable) 
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	void ResetInteractionChecks();

	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionMultiple") 
	TObjectPtr<AActor> ValidActorPatternCallInteract = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionMultiple")
	TArray<TObjectPtr<AActor>> ActorsToCallInteract;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionMultiple") 
	float InterwallTimer = 0.0f;
	
private:
	UPROPERTY() 
	TArray<TScriptInterface<IInteractionInterface>> InteractionsToCallInteract;
	
	UPROPERTY()
	TScriptInterface<IInteractionInterface> ValidInteractionPatternCallInteract = nullptr;
	
	UPROPERTY()
	TArray<bool> InteractionChecks;
	
	UPROPERTY()
	FTimerHandle MyTimerHandle;
};
