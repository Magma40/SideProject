// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Limb.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API ULimb : public UStaticMeshComponent
{
	GENERATED_BODY()

	ULimb();
	
protected:
virtual void BeginPlay() override;
virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
USceneComponent* AttachedParent = nullptr;
USceneComponent* AttachedChild = nullptr;

public:
	UPROPERTY(EditAnywhere, Category=	"Limb")
	FRotator MeshOrientationOffset = FRotator(0.f, 0.f, 0.f);
};
