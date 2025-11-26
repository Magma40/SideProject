// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Joint.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UJoint : public UStaticMeshComponent
{
	GENERATED_BODY()
	
protected:
	UJoint();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	USceneComponent* JointReference = nullptr;
	UPROPERTY(EditAnywhere, Category = "Joint") FString JointName;
};
