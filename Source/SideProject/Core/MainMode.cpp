// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameMode.h"
#include "Tools/GenericCalls.h"
#include "WorldInteractions/InteractionManager.h"

AMainGameMode::AMainGameMode()
{
	// Create the ActionStack component and attach to GameMode
}

UInteractionManager* AMainGameMode::GetInteractionManager()
{
	if (IsValid(InteractionManager)) return InteractionManager;
	UInteractionManager* FoundInteractionManager = GenericCalls::FindChildComponentByClass<UInteractionManager>(this);
	if (!IsValid(FoundInteractionManager))
	{
		UE_LOG(LogTemp, Error, TEXT("%s:GetInteractionManager - InteractionManager not found or wrong type"), *StaticClass()->GetName());
		return nullptr;
	}
	InteractionManager = FoundInteractionManager;
	return InteractionManager;
}
