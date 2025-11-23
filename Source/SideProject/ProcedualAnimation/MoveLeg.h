// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MoveLeg.generated.h"

USTRUCT(BlueprintType)
struct FLegsPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UMoveLeg : public USceneComponent
{
	GENERATED_BODY()

protected:
	UMoveLeg();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category="Leg") float StepTrigger = 2.0f;
	UPROPERTY(EditAnywhere, Category="Leg") float StepTime = 0.5f;
	UPROPERTY(EditAnywhere, Category="Leg") float StepHeight = 2.0f;
	UPROPERTY(EditAnywhere, Category="Leg") float GroundOffset = 0.5f;
	
	FVector LocalGroundPosition = FVector::ZeroVector;
	bool bIsStepping = false;
	float StepElapsed = 0.0f;
	float LiftAmount  = 0.0f;
	FVector LockedFootPosition;
	bool bFootPlanted = true;

	FLegsPose TargetPose = FLegsPose();
	FLegsPose SourcePose = FLegsPose();
	FLegsPose StepTargetPose  = FLegsPose();

	UPROPERTY() TArray<USceneComponent*> LegParts;
	UPROPERTY()TArray<UMoveLeg*> OtherLegs;

public:
	bool IsStepping() const { return bIsStepping; }
	float GetLiftAmount() const { return LiftAmount; }
	const FLegsPose& GetTargetPose() const { return TargetPose; }
	
	FLegsPose GetGroundPose() const;
	bool ShouldStep() const;
	void StartStep();
	void UpdateStep(float DeltaTime);
	
	void UpdateLeg();

private:
	void Init();
};
