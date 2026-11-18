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
	UFUNCTION(BlueprintNativeEvent, Category = "ActionStackLogic") 
	void OnBegin(bool bFirstTime);
	
	UFUNCTION(BlueprintNativeEvent, Category = "ActionStackLogic") 
	void OnUpdate(float DeltaTime);
	
	UFUNCTION(BlueprintNativeEvent, Category = "ActionStackLogic") 
	void OnEnd();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ActionStackLogic") 
	bool IsDone() const;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UActionStack : public USceneComponent
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	UActionStack();
	
	UFUNCTION(BlueprintCallable)
	void PushAction(const TScriptInterface<IActionInterface>& Action);
	
	UFUNCTION(BlueprintCallable) 
	void RemoveAction(const TScriptInterface<IActionInterface>& Action);
	
	UFUNCTION(BlueprintCallable)
	bool Contains(const TScriptInterface<IActionInterface>& Action) const;

	UFUNCTION(BlueprintCallable) 
	void ClearAllActionsExceptIndex(const int SaveIndex);
	
	//~ Begin Getters & Setters
	UFUNCTION(BlueprintPure, Category = "ActionStack") 
	TArray<TScriptInterface<IActionInterface>> GetActionStack()
	{
		return ActionStack;
	}
	
	UFUNCTION(BlueprintPure) 
	TScriptInterface<IActionInterface> GetCurrentAction() const
	{
		return CurrentAction;
	}
	
	static UActionStack* GetMain(UObject* WorldContext = nullptr);
	
	template<typename T>
	UFUNCTION(BlueprintCallable) 
	T* GetAction();

	template<typename T>
	UFUNCTION(BlueprintCallable) 
	TScriptInterface<IActionInterface> GetInterfacedAction(T* InAction);
	//~ End Getters & Setters
	
protected:
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void UpdateActions(float DeltaTime);
	
	FORCEINLINE bool IsEmpty() const;
	
	//FIELDS:
public:
	static UActionStack* Main;
	
private:
	UPROPERTY() 
	TArray<TScriptInterface<IActionInterface>> ActionStack = TArray<TScriptInterface<IActionInterface>>();
	
	TSet<TScriptInterface<IActionInterface>> FirstTimeActions = TSet<TScriptInterface<IActionInterface>>();
	
	UPROPERTY() 
	TScriptInterface<IActionInterface> CurrentAction = nullptr;

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

template<typename T>
TScriptInterface<IActionInterface> UActionStack::GetInterfacedAction(T* InAction)
{
	TScriptInterface<IActionInterface> ActionInterface;
	UObject* ActionObject = Cast<UObject>(InAction);
	
	if (!ActionObject)
	{
		return ActionInterface;
	}
	
	ActionInterface.SetObject(ActionObject);
	ActionInterface.SetInterface(Cast<IActionInterface>(ActionObject));
	
	return ActionInterface;
}

