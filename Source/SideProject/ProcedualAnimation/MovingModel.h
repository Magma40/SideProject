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


/**
 The class for the hips in procedural animation
 */
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

	UFUNCTION(BlueprintCallable) void SetJumpingAnim() const;
	UFUNCTION(BlueprintCallable) void SetArmsWaveAnim() const;
	UFUNCTION(BlueprintCallable) void SetArmsBackHolsterAnim() const;
	UFUNCTION(BlueprintCallable) void SetArmsPointingAnim() const;
	UFUNCTION(BlueprintCallable) void SetArmsHoldingHandsAnim() const;
	UFUNCTION(BlueprintCallable) void RemoveArmsPointingAnim() const;
	
	template<typename TUpperClass>
	TArray<FLimbChain> BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix) const;
	UFUNCTION(BlueprintCallable) USceneComponent* GetRightHand() const;
	UFUNCTION(BlueprintCallable) USceneComponent* GetLeftHand() const;
	UMoveArm* GetRightArm() const;
	UMoveArm* GetLeftArm() const;
	TArray<UMoveLeg*> GetLegs();
	TArray<UMoveArm*> GetArms();
	UFUNCTION(BlueprintCallable) UAnimModelActionStack* GetAnimModelActionStack() const { return AnimModelActionStack; }
	
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
