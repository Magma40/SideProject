// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Leg.generated.h"

USTRUCT(BlueprintType)
struct FLegPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API ULeg : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	ULeg();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category="Leg") float StepTrigger = 2.0f;
	UPROPERTY(EditAnywhere, Category="Leg") float StepTime = 0.5f;
	UPROPERTY(EditAnywhere, Category="Leg") float StepHeight = 2.0f;
	UPROPERTY(EditAnywhere, Category="Leg") float GroundOffset = 0.5f;
	UPROPERTY(EditAnywhere, Category="Leg") FRotator HipRot = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IK") TArray<USceneComponent*> Bones;
	UPROPERTY() FVector LocalGroundPosition = FVector::ZeroVector;
	UPROPERTY() FLegPose TargetPose = FLegPose();
	UPROPERTY() FLegPose SourcePose = FLegPose();

	UPROPERTY() bool bIsStepping = false;
	UPROPERTY() float StepElapsed = 0.0f;
	UPROPERTY() float LiftAmount  = 0.0f;

	UPROPERTY() TArray<ULeg*> OtherLegs;
	
public:
	UFUNCTION(BlueprintPure)
	bool IsStepping() const { return bIsStepping; }

	UFUNCTION(BlueprintPure)
	float GetLiftAmount() const { return LiftAmount; }

	UFUNCTION(BlueprintPure)
	const FLegPose& GetTargetPose() const { return TargetPose; }

	UFUNCTION(BlueprintPure)
	FVector FootBottom() const;
	
	void UpdateLeg();

private:
	FLegPose GetGroundPose() const;
	bool ShouldStep() const;
	void StartStep();
	void UpdateStep(float DeltaTime);
};
