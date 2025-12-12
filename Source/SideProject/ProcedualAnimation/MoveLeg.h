// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProcedualAnimation/MovingModel.h" // This hurts to add
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

	void SwitchState(EAnimState NewState);
	void UpdateState(float DeltaTime);

	void EnterIdle();
	void EnterMoving();
	void EnterJumping();
	void EnterStepping();
	void EnterLanding();
	
	void UpdateIdle(float DeltaTime);
	void UpdateMoving(float DeltaTime);
	void UpdateJumping(float DeltaTime);
	void UpdateStepping(float DeltaTime);
	void UpdateLanding(float DeltaTime);
	
public:
	void SetIdleAnim();
	void SetMovingAnim();
	void SetJumpingAnim();

	void UpdateIK(float DeltaTime);

	FLegsPose GetGroundPose() const;
	const FLegsPose& GetTargetPose() const { return TargetPose; }
	bool IsStepping() const { return bIsStepping; }
	float GetLiftAmount() const { return LiftAmount; }
	
	bool ShouldStep() const;
	void StartStep();
	//void UpdateStep(float DeltaTime);
	//void UpdateLeg();

protected: // Variables
	EAnimState State = EAnimState::None;
	
	bool bIsStepping = false;
	bool bFootPlanted = true;

	bool bIsJumping = false;
	bool bHasLeftGround = false;
	
	float StepElapsed = 0.0f;
	float LiftAmount  = 0.0f;
	float AirElapsedTime = 0.0f;

	float LastStepEndTime = 0.f;

	
	FLegsPose TargetPose = FLegsPose();
	FLegsPose SourcePose = FLegsPose();
	FLegsPose StepTargetPose  = FLegsPose();
	FLegsPose LockedFootPosition = FLegsPose();
	FVector TargetLocal = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite) TArray<USceneComponent*> LegParts;
	UPROPERTY()TArray<UMoveLeg*> OtherLegs;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepTrigger = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepTime = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepHeight = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float GroundOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float FootRangeStepForward = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg IK") float AirRiseSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg IK") float MaxAirRise = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") float StepCooldown = 0.25f;
};
