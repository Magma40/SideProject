// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/ActionStack.h"
#include "Core/MainGameMode.h"
#include "Kismet/GameplayStatics.h"

UActionStack* UActionStack::Main = nullptr;

// Sets default values for this component's properties
UActionStack::UActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UActionStack::BeginPlay()
{
	Super::BeginPlay();
}

void UActionStack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateActions(DeltaTime);
	ApplyPendingChanges();
}

 void UActionStack::UpdateActions(float DeltaTime)
{
	if (IsEmpty()) return;
	bIsUpdating = true;

	// Acquire next action if needed
	if (!CurrentAction && ActionStack.Num() > 0)
	{
		CurrentAction = ActionStack[0];
		if (CurrentAction && IsValid(CurrentAction.GetObject()))
		{
			const bool bFirstTime = !FirstTimeActions.Contains(CurrentAction);
			FirstTimeActions.Add(CurrentAction);
			CurrentAction->Execute_OnBegin(CurrentAction.GetObject(), bFirstTime);
		}
		else CurrentAction = nullptr;
	}

	// Update current action
	if (CurrentAction && IsValid(CurrentAction.GetObject()))
	{
		TScriptInterface<IActionInterface> SafeAction = CurrentAction;
		SafeAction->Execute_OnUpdate(SafeAction.GetObject(), DeltaTime); // Changing this while being updated causes "Exception 0x80000003"

		// Action may have ended or been destroyed during update
		if (SafeAction && IsValid(SafeAction.GetObject()))
		{
			if (ActionStack.Num() > 0 && SafeAction == ActionStack[0])
			{
				if (SafeAction->Execute_IsDone(SafeAction.GetObject()))
				{
					PendingRemovals.Add(SafeAction);
				}
			}
		}
		else CurrentAction = nullptr;
	}

	bIsUpdating = false;
}

bool UActionStack::IsEmpty() const { return !CurrentAction && ActionStack.Num() == 0; }

void UActionStack::PushAction(const TScriptInterface<IActionInterface>& Action)
{
	if (!Action || !IsValid(Action.GetObject())) return;
	PendingPushes.Add(Action);
}

void UActionStack::RemoveAction(TScriptInterface<IActionInterface> Action)
{
	if (!Action) return;
	PendingRemovals.Add(Action);
}

UActionStack* UActionStack::GetMain(UObject* WorldContext)
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
	if (!GM || !GM->ActionStack)
	{
		UE_LOG(LogTemp, Warning, TEXT("UActionStack::GetMain() - GameMode or ActionStack missing!"));
		return nullptr;
	}
	Main = GM->ActionStack;
	return Main;
}

void UActionStack::ApplyPendingChanges()
{
	// Removals
	for (const auto& Action : PendingRemovals)
	{
		if (!Action) continue;

		if (CurrentAction == Action)
		{
			if (IsValid(Action.GetObject()))
			{
				Action->Execute_OnEnd(Action.GetObject());
			}
			CurrentAction = nullptr;
		}
		UE_LOG(LogTemp, Error,TEXT("Action %s Has Been Removed From The %s"),*Action.GetObject()->GetFName().ToString(), *this->GetFName().ToString());
		ActionStack.Remove(Action);
	}

	//Pushes
	for (const auto& Action : PendingPushes)
	{
		if (!Action || !IsValid(Action.GetObject())) continue;
		ActionStack.Remove(Action);
		ActionStack.Insert(Action, 0);
		UE_LOG(LogTemp, Error,TEXT("Action %s Has Been Pushed Onto The %s"),*Action.GetObject()->GetFName().ToString(), *this->GetFName().ToString());
	}

	PendingRemovals.Empty();
	PendingPushes.Empty();
}
