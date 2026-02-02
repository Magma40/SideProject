// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItemBase.generated.h"

UENUM()
enum class EMaterialOverlayStatus : uint8
{
	Active,
	Inactive,
	None
};

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
};


UCLASS()
class SIDEPROJECT_API AInteractableItemBase : public AActor, public IInteractableItemInterface
{
	GENERATED_BODY()

protected:
	AInteractableItemBase();
	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void AttachToComponent_Implementation(USceneComponent* InAttachToComponent);
	virtual void DetachFromComponent_Implementation();
	virtual void EnableMaterialInterface_Implementation();
	virtual void DisableMaterialInterface_Implementation();
	
	virtual TArray<UPrimitiveComponent*>  GetPrimitiveComponentChildren_Implementation();
	virtual TArray<UStaticMeshComponent*>  GetStaticMeshChildren_Implementation();
	void SetOwnerCharacter(ABaseInteractableCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter;  }
	ABaseInteractableCharacter* GetOwnerCharacter() const { return OwnerCharacter; }

	EMaterialOverlayStatus GetNeedOutlineMaterial() const { return MaterialOverlayStatus; }
	void SetNeedOutlineMaterial(const EMaterialOverlayStatus InNeedOutlineMaterial) { MaterialOverlayStatus = InNeedOutlineMaterial; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* OutlineMaterialInterface = nullptr;
private:
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	TArray<UStaticMeshComponent*> StaticMeshComponentChildren;
	EMaterialOverlayStatus MaterialOverlayStatus =  EMaterialOverlayStatus::None;;
};

