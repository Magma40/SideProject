// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "UObject/Interface.h"
#include "ActionStack.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UActionInterface : public UInterface
{
	GENERATED_BODY()
};

class IActionInterface
{
	GENERATED_BODY()

public:

	// Add virtual interface functions here
	virtual void OnBegin(bool bFirstTime) = 0;
	virtual void OnUpdate() = 0;
	virtual void OnEnd() = 0;
	virtual bool IsDone() const = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UActionStack : public USceneComponent
{
	GENERATED_BODY()

protected:
	UActionStack();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UpdateActions();

private:
	// TArray<TScriptInterface<UActionInterface>> ActionStack = TArray<TScriptInterface<UActionInterface>>();
	// TSet<TScriptInterface<UActionInterface>> FirstTimeActions = TSet<TScriptInterface<UActionInterface>>();
	// TScriptInterface<UActionInterface> CurrentAction = nullptr;
	// static UActionStack* Main;

public:
	//TArray<TScriptInterface<UActionInterface>> GetActionStack() { return ActionStack; }
	//TScriptInterface<UActionInterface> GetAction() const { return CurrentAction; }
	
	// bool IsEmpty() const;
	// static UActionStack* GetMain();
	// void PushAction(const TScriptInterface<UActionInterface>& Action);
	//
	// template<typename T>
	// T* GetAction();
	// bool Contains(TScriptInterface<UActionInterface> Action) const { return Action  != nullptr && ActionStack.Contains(Action); }
	// void RemoveAction(TScriptInterface<UActionInterface> Action);
};

// UCLASS(Abstract, Blueprintable) // Why is this needed?
// class SIDEPROJECT_API UAction : public UActionInterface
// {
// 	GENERATED_BODY()
//
// public:
//  	virtual bool IsDone() const { return true; }
// 	virtual void OnBegin(bool bFirstTime) { }
// 	virtual void OnUpdate() { }
// 	virtual void OnEnd() { }
//
// 	//override ToString();
// };
//
// UCLASS(Abstract, Blueprintable)
// class SIDEPROJECT_API UActionBehaviour : public UActionInterface // public MonoBehaviour
// {
// 	GENERATED_BODY()
//
// public:
// 	virtual bool IsDone() const { return true; }
// 	virtual void OnBegin(bool bFirstTime) { }
// 	virtual void OnUpdate() { }
// 	virtual void OnEnd() { }
//
// 	//override ToString();
// };
//
// UCLASS(Abstract, Blueprintable)
// class SIDEPROJECT_API UActionObject : public UActionInterface // public ScriptableObject
// {
// 	GENERATED_BODY()
//
// public:
// 	virtual bool IsDone() const { return true; }
// 	virtual void OnBegin(bool bFirstTime) { }
// 	virtual void OnUpdate() { }
// 	virtual void OnEnd() { }
//
// 	//override ToString();
// };
