// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MovingModel.h" // This hurts to add
#include "MoveArm.generated.h"

USTRUCT(BlueprintType)
struct FArmsPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UMoveArm : public USceneComponent
{
	GENERATED_BODY()

protected:	
	UMoveArm();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init();
	bool ShouldHoldUpHands();

	void SwitchState(EAnimState NewState);
	void UpdateState(float DeltaTime);

	void EnterIdle();
	void EnterMoving();
	void EnterJumping();
	void EnterLanding();
	
	void UpdateIdle(float DeltaTime);
	void UpdateMoving(float DeltaTime);
	void UpdateJumping(float DeltaTime);
	void UpdateLanding(float DeltaTime);

public:
	void UpdateIK();

	void SetIdleAnim();
	void SetMovingAnim();
	void SetJumpingAnim();
	
	void SetWaveAnim();
	void SetBackHolsterAnim(FVector HolsterLocation);
	

private:
	EAnimState State = EAnimState::None;
	
	FArmsPose TargetPose = FArmsPose();
	TArray<USceneComponent*> ArmParts;
	TArray<UMoveArm*> OtherArms;
	bool ActionEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category= "Animation") UCurveFloat* WaveAnimCurve;
	UPROPERTY(EditDefaultsOnly, Category= "Animation") UCurveFloat* HolsterAnimCurve;
};
