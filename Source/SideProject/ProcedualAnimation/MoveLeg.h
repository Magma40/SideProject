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
	
protected: // Functions
	UMoveLeg();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Init();

public:
	void SetIdleAnim();
	void SetMovingAnim();
	void SetJumpingAnim(bool IsJumping, float DeltaTime);
	bool IsStepping() const { return bIsStepping; }
	float GetLiftAmount() const { return LiftAmount; }
	const FLegsPose& GetTargetPose() const { return TargetPose; }
	FLegsPose GetGroundPose() const;
	
	bool ShouldStep() const;
	void StartStep();
	void UpdateStep(float DeltaTime);
	void UpdateLeg();

protected: // Variables
	bool bIsStepping = false;
	float StepElapsed = 0.0f;
	float LiftAmount  = 0.0f;
	bool bFootPlanted = true;
	
	float AirElapsedTime = 0.0f;
	float AirRiseSpeed = 20.0f;
	float MaxAirRise = 60.0f;
	
	bool bIsJumping = false;
	bool bJustLanded = false;
	float LastLandTime = 0.f;

	FLegsPose TargetPose = FLegsPose();
	FLegsPose SourcePose = FLegsPose();
	FLegsPose StepTargetPose  = FLegsPose();
	FLegsPose LockedFootPosition = FLegsPose();
	FVector LockedJumpFootLocal = FVector::ZeroVector;
	FVector TargetLocal = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite) TArray<USceneComponent*> LegParts;
	UPROPERTY()TArray<UMoveLeg*> OtherLegs;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepTrigger = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepTime = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepHeight = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float GroundOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float FootRangeStepForward = 20.0f;
};
