// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "UObject/Interface.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/PublicVariables.h"
#include "InteractionManager.generated.h"

class ABaseInteractableCharacter;

UENUM()
enum class EActionType : uint8
{
	Active,
	Movement,
	Passive
};

UINTERFACE(Blueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void Interact(ABaseInteractableCharacter* InInteractingCharacter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool CanInteract(ABaseInteractableCharacter* InteractingCharacter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) float GetDistanceToInteraction(ABaseInteractableCharacter* InteractingCharacter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void PerformInteraction(ABaseInteractableCharacter* InteractingCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) TArray<UStaticMeshComponent*>  GetAllStaticMeshesInOwner();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void EnableMaterialInterface();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void DisableMaterialInterface(); 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool GetInteractingBool();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) float GetInteractionDistance();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) EActionType GetInteractionType();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) EMaterialOverlayStatus GetNeedOutlineMaterial();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void SetNeedOutlineMaterial(const EMaterialOverlayStatus InNeedOutlineMaterial);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UInteractionManager : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY() TSet<UObject*> Interactions;
	
public:
	TSet<UObject*>  GetInteractions(ABaseInteractableCharacter* InteractingCharacter);
	
	void AddInteraction(UObject* Interaction);
	void RemoveInteraction(const UObject* Interaction);	

	template<typename T>
	T* GetInteraction();
	bool Contains(const UObject* Interaction) const
	{
		return IsValid(Interaction) && IsValid(this) && Interactions.Contains(Interaction);
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

protected:
	EMaterialOverlayStatus MaterialOverlayStatus =  EMaterialOverlayStatus::None;
	EActionType Type = EActionType::Passive;
	ABaseInteractableCharacter* InteractingCharacter = nullptr;
	bool bInteracting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionReferences") UMaterialInterface* OutlineMaterialInterface = nullptr;
	UPROPERTY(EditAnywhere, Category="InteractionReferences") AActor* ActorToCallInteract = nullptr;
	UPROPERTY(EditAnywhere, Category="InteractionReferences") bool bOneUse = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionReferences") 	float DistanceForInteraction = 0.0f;
	
public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)  virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	virtual bool CanInteract_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	virtual float GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	virtual void PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;

	virtual TArray<UStaticMeshComponent*> GetAllStaticMeshesInOwner_Implementation() override;
	virtual void EnableMaterialInterface_Implementation() override;
	virtual void DisableMaterialInterface_Implementation() override;

	virtual bool GetInteractingBool_Implementation() override { return bInteracting; }
	virtual float GetInteractionDistance_Implementation() override { return DistanceForInteraction; } 
	virtual EActionType GetInteractionType_Implementation() override { return Type; }
	virtual EMaterialOverlayStatus GetNeedOutlineMaterial_Implementation() override { return MaterialOverlayStatus; }
	virtual void SetNeedOutlineMaterial_Implementation(const EMaterialOverlayStatus InNeedOutlineMaterial) override { MaterialOverlayStatus = InNeedOutlineMaterial; } 
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

