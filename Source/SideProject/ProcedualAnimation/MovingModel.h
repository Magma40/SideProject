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
	UPROPERTY()  USceneComponent* Middle = nullptr;    // Knee or Elbow
	UPROPERTY()  USceneComponent* End = nullptr;		   // Foot or Hand
};

class UAnimModelActionStack;
class UMoveLeg;
class UMoveArm;

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
	UFUNCTION(BlueprintCallable) void SetArmsHoldingHandsAnim();
	
	template<typename TUpperClass>
	TArray<FLimbChain> BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix) const;
	USceneComponent* GetRightHand() const;
	USceneComponent* GetLeftHand() const;
	TArray<UMoveLeg*> GetLegs();
	TArray<UMoveArm*> GetArms();
	UAnimModelActionStack* GetAnimModelActionStack() const { return AnimModelActionStack; }
	
	void SetAnimModelActionStack(UAnimModelActionStack* InAnimModelActionStack) { AnimModelActionStack = InAnimModelActionStack; } 

private: // Variables
	float BaseHipHeight = 0.0f;
	
	USceneComponent* Hips = nullptr;
	USceneComponent* Chest = nullptr;
	
	UPROPERTY() TArray<FLimbChain> Legs;
	UPROPERTY() TArray<FLimbChain> Arms;

	UAnimModelActionStack* AnimModelActionStack = nullptr;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float HipSwayAmount = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float MoveSpeed = 1.0f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") float RotationSpeed = 100.0f;
};
