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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnBegin(bool bFirstTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnUpdate();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnEnd();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool IsDone() const;
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
	TArray<TScriptInterface<IActionInterface>> ActionStack = TArray<TScriptInterface<IActionInterface>>();
	TSet<TScriptInterface<IActionInterface>> FirstTimeActions = TSet<TScriptInterface<IActionInterface>>();
	TScriptInterface<IActionInterface> CurrentAction = nullptr;
	static UActionStack* Main;

public:
	TArray<TScriptInterface<IActionInterface>> GetActionStack() { return ActionStack; }
	TScriptInterface<IActionInterface> GetCurrentAction() const { return CurrentAction; }
	
	 bool IsEmpty() const;
	 static UActionStack* GetMain();
	 void PushAction(const TScriptInterface<IActionInterface>& Action);
	
	 template<typename T>
	 T* GetAction();
	 bool Contains(TScriptInterface<IActionInterface> Action) const
	 {
		 return Action  != nullptr && ActionStack.Contains(Action);
	 }
	 void RemoveAction(TScriptInterface<IActionInterface> Action);	
};

UCLASS(Abstract, Blueprintable) // Why is this needed?
class SIDEPROJECT_API UAction : public UObject, public IActionInterface
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override {}
	virtual void OnUpdate_Implementation() override {}
	virtual void OnEnd_Implementation() override {}
	virtual bool IsDone_Implementation() const override { return true; }

	//override ToString();
};

UCLASS(Abstract, Blueprintable)
class SIDEPROJECT_API UActionBehaviour : public UObject, public IActionInterface// public MonoBehaviour
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override {}
	virtual void OnUpdate_Implementation() override {}
	virtual void OnEnd_Implementation() override {}
	virtual bool IsDone_Implementation() const override { return true; }

	//override ToString();
};

UCLASS(Abstract, Blueprintable)
class SIDEPROJECT_API UActionObject  : public UObject, public IActionInterface// public ScriptableObject
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override {}
	virtual void OnUpdate_Implementation() override {}
	virtual void OnEnd_Implementation() override {}
	virtual bool IsDone_Implementation() const override { return true; }

	//override ToString();
};

template<typename T>
T* UActionStack::GetAction()
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "T must be a UObject type");
	for (TScriptInterface Action : ActionStack)
	{
		if (Action && Action.GetObject()->IsA(T::StaticClass())) return Cast<T>(Action);
	}
	return nullptr;
}
