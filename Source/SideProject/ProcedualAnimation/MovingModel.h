// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveLeg.h"
#include "MoveArm.h"
#include "GameFramework/Actor.h"
#include "MovingModel.generated.h"


USTRUCT()
struct FLimbChain
{
	GENERATED_BODY()

	UPROPERTY()  USceneComponent* Upper = nullptr;     // UpperLeg or UpperArm
	UPROPERTY()  USceneComponent* Middle = nullptr;    // Knee or Elbow
	UPROPERTY()  USceneComponent* End = nullptr;		   // Foot or Hand
};

class UAnimModelActionStack;
class UMoveLeg;
class UMoveArm;
class ULimb;

UCLASS()
class SIDEPROJECT_API AMovingModel : public AActor
{
	GENERATED_BODY()

protected: // Functions
	AMovingModel();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void DrawDebugJoints();
	void DrawDebugLimbs();

public:
	void InitSceneComponents();
	void UpdateHips();
	float GetHipSway();

	UFUNCTION(BlueprintCallable) void SetLegsJumpingAnim();
	UFUNCTION(BlueprintCallable) void SetArmsWaveAnim();
	UFUNCTION(BlueprintCallable) void SetArmsBackHolsterAnim();
	UFUNCTION(BlueprintCallable) void SetArmsPointingAnim();
	
	template<typename TUpperClass>
	TArray<FLimbChain> BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix) const;
	USceneComponent* GetRightHand() const
	{
		for(FLimbChain Arm : Arms)
		{
			if (Arm.End->GetName().EndsWith(TEXT("_R"))) return Arm.End;
		}
		return nullptr;
	}

	USceneComponent* GetLeftHand() const
	{
		for(FLimbChain Arm : Arms)
		{
			if (Arm.End->GetName().EndsWith(TEXT("_L"))) return Arm.End;
		}
		return nullptr;
	}

	TArray<UMoveLeg*> GetLegs()
	{
		TArray<UMoveLeg*> LegsTops;
		for(FLimbChain Leg : Legs)
		{
			LegsTops.Add(Cast<UMoveLeg>(Leg.Upper));
		}
		return LegsTops;
	}

	TArray<UMoveArm*> GetArms()
	{
		TArray<UMoveArm*> ArmsTops;
		for(FLimbChain Arm : Arms)
		{
			ArmsTops.Add(Cast<UMoveArm>(Arm.Upper));
		}
		return ArmsTops;
	}

protected: // Variables
	float BaseHipHeight = 0.0f;
	
	USceneComponent* Hips = nullptr;
	USceneComponent* Chest = nullptr;
	
	UPROPERTY() TArray<FLimbChain> Legs;
	UPROPERTY() TArray<FLimbChain> Arms;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float HipSwayAmount = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float MoveSpeed = 1.0f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float RotationSpeed = 100.0f;

	UAnimModelActionStack* AnimModelActionStack = nullptr;
};
