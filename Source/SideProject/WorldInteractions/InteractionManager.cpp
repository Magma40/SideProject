// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/InteractionManager.h"

#include "Core/MainGameMode.h"
#include "Kismet/GameplayStatics.h"
void UInteractionManager::AddInteraction(UObject* Interaction)
{
	if (!IsValid(Interaction) && !IsValid(this)) return;
	if(Contains(Interaction)) return;
	UE_LOG(LogTemp, Warning,TEXT("Interaction %s Has Been Added Onto The %s"),*Interaction->GetFName().ToString(), *StaticClass()->GetFName().ToString());
	Interactions.Add(Interaction);
}

void UInteractionManager::RemoveInteraction(const UObject* Interaction)
{
	if (!IsValid(Interaction) && !IsValid(this)) return;
	if(!Contains(Interaction)) return;
	UE_LOG(LogTemp, Error,TEXT("Interaction %s Has Been Removed From The %s"),*Interaction->GetFName().ToString(), *StaticClass()->GetFName().ToString());
	Interactions.Remove(Interaction);
}

TSet<UObject*>  UInteractionManager::GetInteractions(ABaseInteractableCharacter* InteractingCharacter)
{
    TSet<UObject*> Results;
	for (UObject* Obj : Interactions)
	{
		if (!IsValid(Obj)) continue;
		if (!Obj->Implements<UInteractionInterface>()) continue;

		const float Distance =IInteractionInterface::Execute_GetDistanceToInteraction(Obj, InteractingCharacter);
		const float MaxDistance =IInteractionInterface::Execute_GetInteractionDistance(Obj);

		if (Distance < MaxDistance)
		{
			Results.Add(Obj);
		}
	}
	return Results;
}


