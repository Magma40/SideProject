// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/ActionStack.h"

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
	
}

// bool UActionStack::IsEmpty() const
// {
// 	if (CurrentAction == nullptr && ActionStack.Num() == 0) return true;
// 	return false;
// }
//
// UActionStack* UActionStack::GetMain()
// {
// 	if (!IsValid(Main) && GEngine)
// 	{
// 		Main = NewObject<UActionStack>();
// 		Main->AddToRoot();
// 	}
// 	return Main;
// }
//
// void UActionStack::PushAction(const TScriptInterface<UActionInterface>& Action)
// {
// 	if (!Action) return;
// 	ActionStack.RemoveAll([&](const TScriptInterface<UActionInterface>& A){return A.GetObject() == Action.GetObject(); });
// 	ActionStack.Insert(Action, 0);
// 	if (CurrentAction && CurrentAction.GetObject() != Action.GetObject()) { CurrentAction = nullptr; }}
//
// template<typename T>
// T* UActionStack::GetAction()
// {
// 	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "T must be a UObject type");
// 	for (TScriptInterface Action : ActionStack)
// 	{
// 		if (Action && Action->IsA(T::StaticClass())) return Cast<T>(Action);
// 	}
// 	return nullptr;
// }
//
// void UActionStack::RemoveAction(TScriptInterface<UActionInterface> Action)
// {
// 	if (Contains(Action)) return;
// 	if (CurrentAction == Action)
// 	{
// 		//Action->OnEnd();
// 		CurrentAction = nullptr;
// 	}
// 	ActionStack.Remove(Action);
// }

