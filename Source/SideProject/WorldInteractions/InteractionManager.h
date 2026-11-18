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

	//FUNCTIONS:
public:
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	void Interact(ABaseInteractableCharacter* InInteractingCharacter);
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	bool CanInteract(ABaseInteractableCharacter* InteractingCharacter);
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic")
	float GetDistanceToInteraction(ABaseInteractableCharacter* InteractingCharacter);
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	void PerformInteraction(ABaseInteractableCharacter* InteractingCharacter);

	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	TArray<UStaticMeshComponent*>  GetAllStaticMeshesInOwner();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	void EnableMaterialInterface();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	void DisableMaterialInterface(); 

	//~ Begin Getters & Setters
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	FORCEINLINE bool GetInteractingBool();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	FORCEINLINE float GetInteractionDistance();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	FORCEINLINE EActionType GetInteractionType();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	FORCEINLINE EMaterialOverlayStatus GetNeedOutlineMaterial();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionLogic") 
	FORCEINLINE void SetNeedOutlineMaterial(const EMaterialOverlayStatus InNeedOutlineMaterial);
	//~ End Getters & Setters
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UInteractionManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	TSet<UObject*> GetInteractions(ABaseInteractableCharacter* InteractingCharacter);
	
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
		
		if (!ActionObject)
		{
			return ActionInterface;
		}
		
		ActionInterface.SetObject(ActionObject);
		ActionInterface.SetInterface(Cast<IInteractionInterface>(ActionObject));
		
		return ActionInterface;
	}
	
private:
	UPROPERTY() 
	TSet<UObject*> Interactions;
};

UCLASS(Abstract, Blueprintable) 
class SIDEPROJECT_API UInteractionBehaviour : public USceneComponent, public IInteractionInterface
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		enum ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)  
	virtual void Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	virtual bool CanInteract_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	virtual float GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;
	
	virtual void PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter) override;

	virtual TArray<UStaticMeshComponent*> GetAllStaticMeshesInOwner_Implementation() override;
	
	virtual void EnableMaterialInterface_Implementation() override;
	
	virtual void DisableMaterialInterface_Implementation() override;

	//~ Begin Getters & Setters
	FORCEINLINE virtual bool GetInteractingBool_Implementation() override
	{
		return bInteracting;
	}
	
	FORCEINLINE virtual float GetInteractionDistance_Implementation() override
	{
		return DistanceForInteraction;
	} 
	
	FORCEINLINE virtual EActionType GetInteractionType_Implementation() override
	{
		return Type;
	}
	
	FORCEINLINE virtual EMaterialOverlayStatus GetNeedOutlineMaterial_Implementation() override
	{
		return MaterialOverlayStatus;
	}
	
	FORCEINLINE virtual void SetNeedOutlineMaterial_Implementation(const EMaterialOverlayStatus InNeedOutlineMaterial) override
	{
		MaterialOverlayStatus = InNeedOutlineMaterial;
	} 
	//~ End Getters & Setters

	//FIELDS:
protected:
	EMaterialOverlayStatus MaterialOverlayStatus =  EMaterialOverlayStatus::None;
	
	EActionType Type = EActionType::Passive;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> InteractingCharacter = nullptr;
	
	bool bInteracting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionReferences") 
	TObjectPtr<UMaterialInterface> OutlineMaterialInterface = nullptr;
	
	UPROPERTY(EditAnywhere, Category="InteractionReferences") 
	TObjectPtr<AActor> ActorToCallInteract = nullptr;
	
	UPROPERTY(EditAnywhere, Category="InteractionReferences") 
	bool bOneUse = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractionReferences") 
	float DistanceForInteraction = 0.0f;
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

