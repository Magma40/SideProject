// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/ActionStack.h"
#include "Core/MainGameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainCharacter.h"

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
	UpdateActions();
}

 void UActionStack::UpdateActions()
{
	if (IsEmpty()) return;

	while (CurrentAction == nullptr && ActionStack.Num() > 0)
	{
		CurrentAction = ActionStack[0];

		bool bFirstTime = !FirstTimeActions.Contains(CurrentAction);
		FirstTimeActions.Add(CurrentAction);
		CurrentAction->Execute_OnBegin(CurrentAction.GetObject(), bFirstTime);
		if (CurrentAction != nullptr)
		{
			if (ActionStack.Num() > 0 && CurrentAction != ActionStack[0])
			{
				CurrentAction = nullptr;
				UpdateActions();
				return;
			}
		}
	}

	if (CurrentAction != nullptr)
	{
		CurrentAction->Execute_OnUpdate(CurrentAction.GetObject());
		if (ActionStack.Num() > 0 && CurrentAction == ActionStack[0])
		{
			if (CurrentAction->Execute_IsDone(CurrentAction.GetObject()))
			{
				RemoveAction(CurrentAction);
			}
		}
		else
		{
			CurrentAction = nullptr;
		}
	}
	
}

bool UActionStack::IsEmpty() const
{
	if (CurrentAction == nullptr && ActionStack.Num() == 0) return true;
	return false;
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

void UActionStack::PushAction(const TScriptInterface<IActionInterface>& Action)
{
	if (!Action) return;
	ActionStack.RemoveAll([&](const TScriptInterface<IActionInterface>& A)
	{
		  return A.GetObject() == Action.GetObject();
	});
	ActionStack.Insert(Action, 0);
	if (CurrentAction && CurrentAction.GetObject() != Action.GetObject())
	{
		CurrentAction = nullptr;
	}
	UE_LOG(LogTemp, Error,TEXT("Action %s Has Been Pushed Onto The Stack"),*Action.GetObject()->GetFName().ToString());
}


void UActionStack::RemoveAction(TScriptInterface<IActionInterface> Action)
{
	if (!Contains(Action)) return; // This might be wrong?
	ActionStack.RemoveAt(0);
	if (CurrentAction == Action)
	{
		Action->Execute_OnEnd(CurrentAction.GetObject());
		CurrentAction = nullptr;
	}
	ActionStack.Remove(Action);
	UE_LOG(LogTemp, Error,TEXT("Action %s Has Been Removed From The Stack"),*Action.GetObject()->GetFName().ToString());
}

