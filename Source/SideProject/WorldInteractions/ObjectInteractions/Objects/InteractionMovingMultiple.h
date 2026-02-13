#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionMovingMultiple.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API AInteractionMovingMultiple : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	AInteractionMovingMultiple();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)  virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;

private:
	UPROPERTY() TArray<TScriptInterface<IInteractionInterface>> InteractionsToCallInteract;
	UPROPERTY() TScriptInterface<IInteractionInterface> ValidInteractionPatternCallInteract = nullptr;
	UPROPERTY() TScriptInterface<IInteractionInterface> InvalidInteractionPatternCallInteract = nullptr;
	TArray<bool> CurrentPattern;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") AActor* ValidActorPatternCallInteract = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") AActor* InvalidActorPatternCallInteract = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") TArray<AActor*> ActorsToCallInteract;
};
