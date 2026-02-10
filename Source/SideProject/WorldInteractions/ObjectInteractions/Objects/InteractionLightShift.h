#pragma once

#include "CoreMinimal.h"
#include "WorldInteractions/InteractionManager.h"
#include "InteractionLightShift.generated.h"

class USpotLightComponent;
class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API AInteractionLightShift : public AActor,  public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	AInteractionLightShift();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)  virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;

private:
	bool bShouldLight = false;
	USpotLightComponent* SpotLight = nullptr;


public:
	UPROPERTY(EditAnywhere, Category="LightShift")float MaxIntensity = 160.f;
	UPROPERTY(EditAnywhere, Category="LightShift") float LightSpeed = 10.f;
	UPROPERTY(EditAnywhere, Category="LightShift") AActor* ActorToCallInteract = nullptr;
};
