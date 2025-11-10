// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Foot.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UFoot : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UFoot();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Equivalent of Unity's AnimationCurve
	UPROPERTY(EditAnywhere, Category="Foot")
	FRuntimeFloatCurve ToeBendCurve;

private:
	void UpdateFoot();

	TArray<USceneComponent*> ToeComponents;
};
