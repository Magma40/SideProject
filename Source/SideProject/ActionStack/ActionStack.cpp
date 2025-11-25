// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/ActionStack.h"

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
		CurrentAction->OnBegin(bFirstTime);
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
		CurrentAction->OnUpdate();
		if (ActionStack.Num() > 0 && CurrentAction == ActionStack[0])
		{
			if (CurrentAction->IsDone())
			{
				ActionStack.RemoveAt(0);
				CurrentAction->OnEnd();
				FirstTimeActions.Remove(CurrentAction);
				CurrentAction = nullptr;
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

UActionStack* UActionStack::GetMain()
{
	if (!IsValid(Main) && GEngine)
	{
		Main = NewObject<UActionStack>();
		Main->AddToRoot();
	}
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
	if (CurrentAction && CurrentAction.GetObject() != Action.GetObject()) { CurrentAction = nullptr; }
}


void UActionStack::RemoveAction(TScriptInterface<IActionInterface> Action)
{
	if (Contains(Action)) return; // This might be wrong?
	if (CurrentAction == Action)
	{
		Action->OnEnd();
		CurrentAction = nullptr;
	}
	ActionStack.Remove(Action);
}

