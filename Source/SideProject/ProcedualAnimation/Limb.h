// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Limb.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API ULimb : public UStaticMeshComponent
{
	GENERATED_BODY()
	
protected:
	ULimb();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	USceneComponent* AttachedParent = nullptr;
	USceneComponent* AttachedChild = nullptr;
	UPROPERTY(EditAnywhere, Category = "Limb") FString AttachedParentName;
	UPROPERTY(EditAnywhere, Category = "Limb") FString AttachedChildName; 
};
