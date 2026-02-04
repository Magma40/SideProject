// Fill out your copyright notice in the Description page of Project Settings.

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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void AttachToComponent(USceneComponent* InAttachToComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void DetachFromComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void EnableMaterialInterface();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void DisableMaterialInterface();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) TArray<UPrimitiveComponent*>  GetPrimitiveComponentChildren();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) TArray<UStaticMeshComponent*>  GetStaticMeshChildren();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) EMaterialOverlayStatus GetNeedOutlineMaterial();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void SetNeedOutlineMaterial(const EMaterialOverlayStatus InNeedOutlineMaterial);
};


UCLASS()
class SIDEPROJECT_API AInteractableItemBase : public AActor, public IInteractableItemInterface
{
	GENERATED_BODY()

protected:
	AInteractableItemBase();
	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void AttachToComponent_Implementation(USceneComponent* InAttachToComponent) override;
	virtual void DetachFromComponent_Implementation() override;
	virtual void EnableMaterialInterface_Implementation() override;
	virtual void DisableMaterialInterface_Implementation() override;
	
	virtual TArray<UPrimitiveComponent*>  GetPrimitiveComponentChildren_Implementation() override;
	virtual TArray<UStaticMeshComponent*>  GetStaticMeshChildren_Implementation() override;
	void SetOwnerCharacter(ABaseInteractableCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter;  }
	ABaseInteractableCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
	bool GetPersistentlyFrozen() const { return bPersistentlyFrozen; }
	void SetPersistentlyFrozen(const bool InPersistentlyFrozen) {  bPersistentlyFrozen = InPersistentlyFrozen; }
	
	virtual EMaterialOverlayStatus GetNeedOutlineMaterial_Implementation() override  { return MaterialOverlayStatus; } 
	virtual void SetNeedOutlineMaterial_Implementation(const EMaterialOverlayStatus InNeedOutlineMaterial) override { MaterialOverlayStatus = InNeedOutlineMaterial; } 

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* OutlineMaterialInterface = nullptr;
	
protected:
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	TArray<UStaticMeshComponent*> StaticMeshComponentChildren;
	EMaterialOverlayStatus MaterialOverlayStatus =  EMaterialOverlayStatus::None;
	bool bPersistentlyFrozen = false;
};

