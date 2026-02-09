#include "InteractionLightShift.h"

#include "Components/SpotLightComponent.h"
#include "Tools/GenericCalls.h"

AInteractionLightShift::AInteractionLightShift() { PrimaryActorTick.bCanEverTick = true; }

void AInteractionLightShift::BeginPlay()
{
	Super::BeginPlay();
	TArray<USpotLightComponent*> Components;
	GetComponents<USpotLightComponent>(Components);
	if (Components.IsEmpty()) return;
	SpotLight = Components[0];
}

void AInteractionLightShift::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(SpotLight)) return;
	const float TargetIntensity = bShouldLight ? MaxIntensity : 0.f;
	const float NewIntensity = FMath::FInterpTo(SpotLight->Intensity, TargetIntensity, DeltaTime,LightSpeed);

	SpotLight->SetIntensity(NewIntensity);
	bShouldLight = false;
}

void AInteractionLightShift::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	bShouldLight = true;
	if (IsValid(ActorToCallInteract))
	{
		UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ActorToCallInteract);
		if (IsValid(InteractObject)) InteractObject->Interact_Implementation(InInteractingCharacter);

		if (ActorToCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			Cast<IInteractionInterface>(ActorToCallInteract)->Interact_Implementation(InInteractingCharacter);
		}
	}
}
