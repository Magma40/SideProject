// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Debug/ReporterGraph.h"
#include "UObject/Interface.h"
#include "InteractionManager.generated.h"

class ABaseInteractableCharacter;

UENUM()
enum class EActionType : uint8
{
	Active,
	Movement,
	Passive
};

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractionInterface
{
	GENERATED_BODY()

public:
	EActionType Type;
	float DistanceForInteraction;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool CanInteract(ABaseInteractableCharacter* InteractingCharacter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) float GetDistanceToInteraction(ABaseInteractableCharacter* InteractingCharacter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void PerformInteraction(ABaseInteractableCharacter* InteractingCharacter);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractionManager : public USceneComponent
{
	GENERATED_BODY()

protected:
	UInteractionManager();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TSet<TScriptInterface<IInteractionInterface>> Interactions = TSet<TScriptInterface<IInteractionInterface>>();
	
public:
	static UInteractionManager* Main;
	static UInteractionManager* GetMain(UObject* WorldContext = nullptr);

	TSet<TScriptInterface<IInteractionInterface>> GetInteractions(ABaseInteractableCharacter* InteractingCharacter);
	
	void AddInteraction(const TScriptInterface<IInteractionInterface>& Interaction);
	void RemoveInteraction(const TScriptInterface<IInteractionInterface>& Interaction);	

	template<typename T>
	T* GetInteraction();
	bool Contains(const TScriptInterface<IInteractionInterface>& Interaction) const
	{
		return Interaction  != nullptr && Interactions.Contains(Interaction);
	}

	template<typename T>
	TScriptInterface<IInteractionInterface> GetInterfacedAction(T* InAction)
	{
		TScriptInterface<IInteractionInterface> ActionInterface;
		UObject* ActionObject = Cast<UObject>(InAction);
		if (!ActionObject) return ActionInterface;
		ActionInterface.SetObject(ActionObject);
		ActionInterface.SetInterface(Cast<IInteractionInterface>(ActionObject));
		return ActionInterface;
	}
};

UCLASS(Abstract, Blueprintable) 
class SIDEPROJECT_API UInteractionBehaviour : public USceneComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:
	EActionType Type;
	float DistanceForInteraction;
	
	virtual bool CanInteract_Implementation(ABaseInteractableCharacter* InteractingCharacter) override { return false; }
	virtual float GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override { return 0.0f;}
	virtual void PerformInteraction_Implementation(ABaseInteractableCharacter* InteractingCharacter) override {}

// protected:
// 	UInteractionBehaviour() { UInteractionManager::GetMain()->AddInteraction(this); }
// 	virtual ~UInteractionBehaviour() override { UInteractionManager::GetMain()->RemoveInteraction(this); }
};

template<typename T>
T* UInteractionManager::GetInteraction()
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "T must be a UObject type");
	for (const TScriptInterface<IInteractionInterface>& Action : Interactions)
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

