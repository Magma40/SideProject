// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingModel.generated.h"


USTRUCT()
struct FLimbChain
{
	GENERATED_BODY()

	USceneComponent* Upper = nullptr;     // or UMoveArm for arms
	USceneComponent* Middle = nullptr; // Knee or Elbow
	USceneComponent* End = nullptr;    // Foot or Hand
};

class UMoveLeg;
class UMoveArm;

UCLASS()
class SIDEPROJECT_API AMovingModel : public APawn
{
	GENERATED_BODY()

protected:
	AMovingModel();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Init();
	void UpdateHips();
	void UpdateMovement(float DeltaTime);
	
	void DrawDebugJoints();
	void DrawDebugLimbs();

	USceneComponent* FindChildByExactName(USceneComponent* Parent, const FString& Target);
	TArray<USceneComponent*> FindChildrenByPrefix(USceneComponent* Parent, const FString& Prefix);
	template<typename TUpperClass>
	TArray<FLimbChain> BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix);
private:
	//Variables
	float BaseHipHeight = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Hips") float HipSwayAmount = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Hips") float MoveSpeed = 800.0f; 
	UPROPERTY(EditAnywhere, Category = "Hips") float RotationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Hips") float HipHeightScale = 1.0f;
	
	//Objects
	AActor* MoveTarget = nullptr;
	USceneComponent* Hips = nullptr;
	USceneComponent* Chest = nullptr;
	
	TArray<FLimbChain> Legs;
	TArray<FLimbChain> Arms;
};
