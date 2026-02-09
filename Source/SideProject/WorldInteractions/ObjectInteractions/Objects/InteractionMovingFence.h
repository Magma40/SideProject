#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionMovingFence.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API AInteractionMovingFence : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	AInteractionMovingFence();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)  virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	TArray<UStaticMeshComponent*>  GetAllStaticMeshesInOwner_Implementation() override;

private:
	bool bShouldOpen = false;
	TArray<FTransform> AnimTransforms;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MovingFence") float ZMoveTo = 0.0f;
	UPROPERTY(EditAnywhere, Category="MovingFence") FComponentReference ComponentsToMoveRef;
};
