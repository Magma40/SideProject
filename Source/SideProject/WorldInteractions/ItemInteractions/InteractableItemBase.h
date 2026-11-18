#pragma once

#include "CoreMinimal.h"
#include "Core/PublicVariables.h"
#include "InteractableItemBase.generated.h"

class ABaseInteractableCharacter;

UINTERFACE(Blueprintable)
class UInteractableItemInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic")
	void AttachToComponent(USceneComponent* InAttachToComponent);
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic")
	void DetachFromComponent();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic") 
	void EnableMaterialInterface();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic")
	void DisableMaterialInterface();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic")
	TArray<UPrimitiveComponent*>  GetPrimitiveComponentChildren();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic")
	TArray<UStaticMeshComponent*>  GetStaticMeshChildren();

	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic") 
	EMaterialOverlayStatus GetNeedOutlineMaterial();
	
	UFUNCTION(BlueprintNativeEvent, Category = "InteractableItemLogic") 
	void SetNeedOutlineMaterial(const EMaterialOverlayStatus InNeedOutlineMaterial);
};

UCLASS()
class SIDEPROJECT_API AInteractableItemBase : public AActor, public IInteractableItemInterface
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	AInteractableItemBase();
	
	virtual void AttachToComponent_Implementation(USceneComponent* InAttachToComponent) override;
	
	virtual void DetachFromComponent_Implementation() override;
	
	virtual void EnableMaterialInterface_Implementation() override;
	
	virtual void DisableMaterialInterface_Implementation() override;
	
	//~ Begin Getters & Setters
	virtual TArray<UPrimitiveComponent*> GetPrimitiveComponentChildren_Implementation() override;
	
	virtual TArray<UStaticMeshComponent*> GetStaticMeshChildren_Implementation() override;
	
	FORCEINLINE void SetOwnerCharacter(ABaseInteractableCharacter* InOwnerCharacter)
	{
		OwnerCharacter = InOwnerCharacter;
	}
	
	FORCEINLINE ABaseInteractableCharacter* GetOwnerCharacter() const
	{
		return OwnerCharacter;
	}
	
	FORCEINLINE bool GetPersistentlyFrozen() const
	{
		return bPersistentlyFrozen;
	}
	
	FORCEINLINE void SetPersistentlyFrozen(const bool InPersistentlyFrozen)
	{
		bPersistentlyFrozen = InPersistentlyFrozen;
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
	
protected:
	virtual void Tick(float DeltaTime) override;
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	TObjectPtr<UMaterialInterface> OutlineMaterialInterface = nullptr;
	
protected:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TArray<UStaticMeshComponent*> StaticMeshComponentChildren;
	
	EMaterialOverlayStatus MaterialOverlayStatus =  EMaterialOverlayStatus::None;
	
	bool bPersistentlyFrozen = false;
};

