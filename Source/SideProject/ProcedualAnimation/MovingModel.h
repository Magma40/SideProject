// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingModel.generated.h"

USTRUCT()
struct FLimbChain
{
	GENERATED_BODY()

	UPROPERTY()  USceneComponent* Upper = nullptr;     // UpperLeg or UpperArm
	UPROPERTY()  USceneComponent* Middle = nullptr;   // Knee or Elbow
	UPROPERTY()  USceneComponent* End = nullptr;		 // Foot or Hand
};

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

	void InitSceneComponents();
	void DrawDebugJoints();
	void DrawDebugLimbs();

public:
	void UpdateHips(float DeltaTime);
	float UpdateLegs(float DeltaTime);
	void UpdateArms();

	UFUNCTION(BlueprintCallable) void SetLegsIdleAnim();
	UFUNCTION(BlueprintCallable) void SetLegsMovingAnim();
	UFUNCTION(BlueprintCallable) void SetLegsJumpingAnim();

	UFUNCTION(BlueprintCallable) void SetArmsIdleAnim();
	UFUNCTION(BlueprintCallable) void SetArmsMovingAnim();
	UFUNCTION(BlueprintCallable) void SetArmsWaveAnim();
	UFUNCTION(BlueprintCallable) void SetArmsBackHolsterAnim();
	
	static USceneComponent* FindChildByExactName(const USceneComponent* Parent, const FString& Target);
	static TArray<USceneComponent*> FindChildrenByPrefix(const USceneComponent* Parent, const FString& Prefix);
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

protected: // Variables
	float BaseHipHeight = 0.0f;
	
	USceneComponent* Hips = nullptr;
	USceneComponent* Chest = nullptr;
	
	UPROPERTY()  TArray<FLimbChain> Legs;
	UPROPERTY() TArray<FLimbChain> Arms;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float HipSwayAmount = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float MoveSpeed = 1.0f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float RotationSpeed = 100.0f;
};
