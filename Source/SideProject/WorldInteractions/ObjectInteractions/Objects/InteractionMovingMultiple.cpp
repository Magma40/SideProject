#include "InteractionMovingMultiple.h"

#include "InteractionLightShift.h"
#include "NNETypes.h"
#include "Tools/GenericCalls.h"

AInteractionMovingMultiple::AInteractionMovingMultiple() { PrimaryActorTick.bCanEverTick = true;  }

void AInteractionMovingMultiple::BeginPlay()
{
	Super::BeginPlay();
	if (ValidActorPatternCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		ScriptInterface.SetObject(ValidActorPatternCallInteract);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(ValidActorPatternCallInteract));

		ValidInteractionPatternCallInteract = ScriptInterface;
	}
	UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ValidActorPatternCallInteract);
	if (IsValid(InteractObject) && InteractObject->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		ScriptInterface.SetObject(InteractObject);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(InteractObject));

		ValidInteractionPatternCallInteract = ScriptInterface;
	}

	if (InvalidActorPatternCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		ScriptInterface.SetObject(InvalidActorPatternCallInteract);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(InvalidActorPatternCallInteract));

		InvalidInteractionPatternCallInteract = ScriptInterface;
	}
	InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(InvalidActorPatternCallInteract);
	if (IsValid(InteractObject) && InteractObject->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		ScriptInterface.SetObject(InteractObject);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(InteractObject));

		InvalidInteractionPatternCallInteract = ScriptInterface;
	}
	
	if (ActorsToCallInteract.IsEmpty()) return;
	for (AActor* ActorToCallInteract : ActorsToCallInteract)
	{
		if (!IsValid(ActorToCallInteract)) continue;
		if (ActorToCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			TScriptInterface<IInteractionInterface> ScriptInterface;
			ScriptInterface.SetObject(ActorToCallInteract);
			ScriptInterface.SetInterface(Cast<IInteractionInterface>(ActorToCallInteract));

			InteractionsToCallInteract.Add(ScriptInterface);
		}

		// Case 2: Check for component implementing interface
		InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ActorToCallInteract);

		if (IsValid(InteractObject) && InteractObject->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			TScriptInterface<IInteractionInterface> ScriptInterface;
			ScriptInterface.SetObject(InteractObject);
			ScriptInterface.SetInterface(Cast<IInteractionInterface>(InteractObject));

			InteractionsToCallInteract.Add(ScriptInterface);
		}
	}
	CurrentPattern.SetNum(InteractionsToCallInteract.Num());
}

void AInteractionMovingMultiple::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	AInteractionLightShift* LightShift = nullptr;
	for (TScriptInterface<IInteractionInterface> ScriptInterface : InteractionsToCallInteract)
	{
		if (!IsValid(ScriptInterface.GetObject())) continue;
		if (Cast<AInteractionLightShift>(ScriptInterface.GetObject()) && Cast<AInteractionLightShift>(ScriptInterface.GetObject())->GetShouldLight())
		{
			if (IsValid(LightShift)) { LightShift = nullptr; return; }
			LightShift = Cast<AInteractionLightShift>(ScriptInterface.GetObject());
		}
	}
	if (IsValid(LightShift))
	{
		for (int i = 0; i < InteractionsToCallInteract.Num(); i++)
		{
			TScriptInterface<IInteractionInterface> ScriptInterface = InteractionsToCallInteract[i];
			if (!IsValid(ScriptInterface.GetObject())) continue;
			if (ScriptInterface.GetObject() == LightShift)
			{
				if (i <= InteractionsToCallInteract.Num() - 1)
				{
					bool AllAreAlright = true;
					if (CurrentPattern.Contains(false)) AllAreAlright = false;
					if (AllAreAlright)
					{
						if (IsValid(ValidInteractionPatternCallInteract.GetObject()))
						{
							ValidInteractionPatternCallInteract->Interact_Implementation(InInteractingCharacter);
						}
					}
				}
				
				if (CurrentPattern[i] == true) continue;
				if (i == 0 && CurrentPattern[0] == false) CurrentPattern[0] = true;
				else if (i != 0 && CurrentPattern[i - 1] == true && CurrentPattern[i] == false) CurrentPattern[i] = true;
				else
				{
					if (IsValid(InvalidInteractionPatternCallInteract.GetObject()))
					{
						InvalidInteractionPatternCallInteract->Interact_Implementation(InInteractingCharacter);
					}
					for (int y = 0; y < CurrentPattern.Num(); y++)
					{
						CurrentPattern[y] = false;
					}
				}
			}
		}
	}
}