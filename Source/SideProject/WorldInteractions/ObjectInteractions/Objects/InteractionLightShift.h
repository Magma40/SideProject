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
	
	AInteractionLightShift();
	
	//FUNCTIONS:
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)  
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;

	//~ Begin Getters & Setters
	bool GetShouldLight() const
	{
		return bShouldLight;
	}
	//~ End Getters & Setters
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, Category="LightShift")
	float MaxIntensity = 160.f;
	
	UPROPERTY(EditAnywhere, Category="LightShift") 
	float LightSpeed = 10.f;
	
	UPROPERTY(EditAnywhere, Category="LightShift")
	TObjectPtr<AActor> ActorToCallInteract = nullptr;

private:
	bool bShouldLight = false;
	
	UPROPERTY()
	TObjectPtr<USpotLightComponent> SpotLight = nullptr;
};
