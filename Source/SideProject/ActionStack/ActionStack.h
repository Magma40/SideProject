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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnUpdate(float DeltaTime);
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
	virtual void UpdateActions(float DeltaTime);
	bool IsEmpty() const;


private:
	UPROPERTY() TArray<TScriptInterface<IActionInterface>> ActionStack = TArray<TScriptInterface<IActionInterface>>();
	TSet<TScriptInterface<IActionInterface>> FirstTimeActions = TSet<TScriptInterface<IActionInterface>>();
	UPROPERTY() TScriptInterface<IActionInterface> CurrentAction = nullptr;
	
public:
	static UActionStack* Main;
	UFUNCTION(BlueprintCallable) TArray<TScriptInterface<IActionInterface>> GetActionStack() { return ActionStack; }
	UFUNCTION(BlueprintCallable) TScriptInterface<IActionInterface> GetCurrentAction() const { return CurrentAction; }
	
	static UActionStack* GetMain(UObject* WorldContext = nullptr);
	UFUNCTION(BlueprintCallable) void PushAction(const TScriptInterface<IActionInterface>& Action);
	UFUNCTION(BlueprintCallable) void RemoveAction(const TScriptInterface<IActionInterface>& Action);	
	
	 template<typename T>
	 UFUNCTION(BlueprintCallable) T* GetAction();
	 UFUNCTION(BlueprintCallable) bool Contains(const TScriptInterface<IActionInterface>& Action) const
	 {
		 return Action  != nullptr && ActionStack.Contains(Action);
	 }

	template<typename T>
	UFUNCTION(BlueprintCallable) TScriptInterface<IActionInterface> GetInterfacedAction(T* InAction)
	{
	 	TScriptInterface<IActionInterface> ActionInterface;
	 	UObject* ActionObject = Cast<UObject>(InAction);
	 	if (!ActionObject) return ActionInterface;
	 	ActionInterface.SetObject(ActionObject);
	 	ActionInterface.SetInterface(Cast<IActionInterface>(ActionObject));
	 	return ActionInterface;
	 }

	UFUNCTION(BlueprintCallable) void ClearAllActionsExceptIndex(const int SaveIndex);
};

UCLASS(Abstract, Blueprintable) 
class SIDEPROJECT_API UAction : public UObject, public IActionInterface
{
	GENERATED_BODY()

public:
	virtual void OnBegin_Implementation(bool bFirstTime) override {}
	virtual void OnUpdate_Implementation(float DeltaTime) override {}
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
	virtual void OnUpdate_Implementation(float DeltaTime) override {}
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
	virtual void OnUpdate_Implementation(float DeltaTime) override {}
	virtual void OnEnd_Implementation() override {}
	virtual bool IsDone_Implementation() const override { return true; }

	//override ToString();
};

template<typename T>
T* UActionStack::GetAction()
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "T must be a UObject type");
	for (const TScriptInterface<IActionInterface>& Action : ActionStack)
	{
		if (Action)
		{
			if (T* TypedAction = Cast<T>(Action.GetObject()))
			{
				return TypedAction;
			}
		}
	}
	return nullptr;
}

