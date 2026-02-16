#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionMultiple.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API AInteractionMultiple : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	AInteractionMultiple();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)  virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	void ResetInteractionChecks();

private:
	UPROPERTY() TArray<TScriptInterface<IInteractionInterface>> InteractionsToCallInteract;
	UPROPERTY() TScriptInterface<IInteractionInterface> ValidInteractionPatternCallInteract = nullptr;
	TArray<bool> InteractionChecks;
	FTimerHandle MyTimerHandle;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") AActor* ValidActorPatternCallInteract = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") TArray<AActor*> ActorsToCallInteract;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence")  float InterwallTimer = 0.0f;
};
