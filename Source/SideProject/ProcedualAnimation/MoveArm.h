// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
//#include "ProcedualAnimation/MovingModel.h" // This hurts to add
#include "Tools/GenericCalls.h"
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

protected: //Functions
	UMoveArm();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init();

public:
	void UpdateIK();
	void SetBackHolsterAnim(FVector HolsterLocation);
	bool ShouldHoldUpHands();

public: // Variables
	FArmsPose TargetPose = FArmsPose();
	TArray<USceneComponent*> ArmParts;
	TArray<UMoveArm*> OtherArms;
	bool ActionEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category= "Animation") UCurveFloat* WaveAnimCurve;
};
