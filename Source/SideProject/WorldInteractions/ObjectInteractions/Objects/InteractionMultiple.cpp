#include "InteractionMultiple.h"
#include "InteractionLightShift.h"
#include "Tools/GenericCalls.h"

AInteractionMultiple::AInteractionMultiple() { PrimaryActorTick.bCanEverTick = true;  }

void AInteractionMultiple::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(ValidActorPatternCallInteract) && ValidActorPatternCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		ScriptInterface.SetObject(ValidActorPatternCallInteract);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(ValidActorPatternCallInteract));

		ValidInteractionPatternCallInteract = ScriptInterface;
	}
	if (!ValidInteractionPatternCallInteract)
	{
		UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ValidActorPatternCallInteract);
		if (IsValid(InteractObject) && InteractObject->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			TScriptInterface<IInteractionInterface> ScriptInterface;
			ScriptInterface.SetObject(InteractObject);
			ScriptInterface.SetInterface(Cast<IInteractionInterface>(InteractObject));

			ValidInteractionPatternCallInteract = ScriptInterface;
		}	
	}
	
	if (ActorsToCallInteract.IsEmpty()) return;
	for (AActor* ActorToCallInteract : ActorsToCallInteract)
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		if (!IsValid(ActorToCallInteract)) continue;
		if (ActorToCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			ScriptInterface.SetObject(ActorToCallInteract);
			ScriptInterface.SetInterface(Cast<IInteractionInterface>(ActorToCallInteract));

			InteractionsToCallInteract.Add(ScriptInterface);
		}
		if (ScriptInterface) continue;
		// Case 2: Check for component implementing interface
		UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ActorToCallInteract);

		if (IsValid(InteractObject) && InteractObject->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			ScriptInterface.SetObject(InteractObject);
			ScriptInterface.SetInterface(Cast<IInteractionInterface>(InteractObject));

			InteractionsToCallInteract.Add(ScriptInterface);
		}
	}
	InteractionChecks.SetNum(InteractionsToCallInteract.Num());
}

void AInteractionMultiple::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	for (int i = 0; i < InteractionsToCallInteract.Num(); i++)
	{
		TScriptInterface<IInteractionInterface> ScriptInterface = InteractionsToCallInteract[i];
		if (!IsValid(ScriptInterface.GetObject())) continue;
		if (ScriptInterface->GetInteractingBool_Implementation())
		{
			InteractionChecks[i] = true;
			if (InterwallTimer != 0.0f)
			{
				GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &AInteractionMultiple::ResetInteractionChecks, InterwallTimer, false);
			}
		}
	}
	
	if (!InteractionChecks.Contains(false))
	{
		if (IsValid(ValidInteractionPatternCallInteract.GetObject()))
		{
			ValidInteractionPatternCallInteract->Interact_Implementation(InInteractingCharacter);
			Destroy(false);
		}
	}
}

void AInteractionMultiple::ResetInteractionChecks()
{
	for (int x = 0; x < InteractionChecks.Num(); x++)
	{
		InteractionChecks[x] = false;
	}
}
