// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/InteractionManager.h"

#include "Core/MainGameMode.h"
#include "Kismet/GameplayStatics.h"

UInteractionManager* UInteractionManager::Main = nullptr;

// Sets default values for this component's properties
UInteractionManager::UInteractionManager() { PrimaryComponentTick.bCanEverTick = true; }

void UInteractionManager::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}

void UInteractionManager::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractionManager::AddInteraction(const TScriptInterface<IInteractionInterface>& Interaction)
{
	if (!Interaction || !IsValid(Interaction.GetObject())) return;
	if(Contains(Interaction.GetObject())) return;
	UE_LOG(LogTemp, Warning,TEXT("Interaction %s Has Been Added Onto The %s"),*Interaction.GetObject()->GetFName().ToString(), *this->GetFName().ToString());
	Interactions.Add(Interaction);
}

void UInteractionManager::RemoveInteraction(const TScriptInterface<IInteractionInterface>& Interaction)
{
	if (!Interaction || !IsValid(Interaction.GetObject())) return;
	if(!Contains(Interaction.GetObject())) return;
	UE_LOG(LogTemp, Error,TEXT("Interaction %s Has Been Removed From The %s"),*Interaction.GetObject()->GetFName().ToString(), *this->GetFName().ToString());
	Interactions.Remove(Interaction);
}

UInteractionManager* UInteractionManager::GetMain(UObject* WorldContext)
{
	if (IsValid(Main)) return Main;
	UWorld* World = nullptr;
	if (WorldContext)
	{
		World = WorldContext->GetWorld();
	}
	else if (GEngine && GEngine->GetWorldContexts().Num() > 0)
	{
		World = GEngine->GetWorldContexts()[0].World();
	}
	
	if (!World) return nullptr;
	AMainGameMode* GM = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GM || !GM->GetInteractionManager())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractionManager::GetMain() - GameMode or InteractionManager missing!"));
		return nullptr;
	}
	Main = GM->GetInteractionManager();
	return GM->InteractionManager;
}

TSet<TScriptInterface<IInteractionInterface>> UInteractionManager::GetInteractions(ABaseInteractableCharacter* InteractingCharacter)
{
	TSet<TScriptInterface<IInteractionInterface>> Results = TSet<TScriptInterface<IInteractionInterface>>();
	for (auto It = Interactions.begin(); It != Interactions.end(); ++It)
	{
		UInteractionBehaviour* ActionRef = Cast<UInteractionBehaviour>(It->GetObject());
		if(ActionRef->GetDistanceToInteraction_Implementation(InteractingCharacter) < ActionRef->DistanceForInteraction)
		{
			Results.Add(*It);
		}
	}
	return Results;
}


