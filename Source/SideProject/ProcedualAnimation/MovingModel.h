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
	UPROPERTY()  USceneComponent* Middle = nullptr; // Knee or Elbow
	UPROPERTY()  USceneComponent* End = nullptr;    // Foot or Hand
};

class UMoveLeg;
class UMoveArm;
class ULimb;

UCLASS()
class SIDEPROJECT_API AMovingModel : public APawn
{
	GENERATED_BODY()

protected:
	AMovingModel();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void InitSceneComponents();
	void UpdateHips(float DeltaTime);
	void UpdateMovement(float DeltaTime);
	
	void DrawDebugJoints();
	void DrawDebugLimbs();

public:
	static USceneComponent* FindChildByExactName(const USceneComponent* Parent, const FString& Target);
	static TArray<USceneComponent*> FindChildrenByPrefix(const USceneComponent* Parent, const FString& Prefix);
	template<typename TUpperClass>
	 TArray<FLimbChain> BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix) const;
	
private:
	//Variables
	float BaseHipHeight = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Hips") float HipSwayAmount = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Hips") float MoveSpeed = 800.0f; 
	UPROPERTY(EditAnywhere, Category = "Hips") float RotationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Hips") float HipHeightScale = 1.0f;
	
	//Objects
	UPROPERTY()  AActor* MoveTarget = nullptr;
	USceneComponent* Hips = nullptr;
	USceneComponent* Chest = nullptr;
	
	UPROPERTY()  TArray<FLimbChain> Legs;
	UPROPERTY() TArray<FLimbChain> Arms;
};
