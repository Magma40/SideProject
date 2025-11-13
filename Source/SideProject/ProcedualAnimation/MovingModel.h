// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingModel.generated.h"

class UMoveLeg;

UCLASS()
class SIDEPROJECT_API AMovingModel : public AActor
{
	GENERATED_BODY()

protected:
	AMovingModel();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Init();
	void UpdateHips(float DeltaTime);
	
	void DrawDebugJoints();
	void DrawDebugLimbs();

private:
	//Variables
	float BaseHipHeight = 0.0f;
	float HipSwayAmount = 0.0f;
	
	//Objects
	USceneComponent* Hips = nullptr;
	TArray<UMoveLeg*>  Legs;

	USceneComponent* UpperLeg_R = nullptr;
	USceneComponent* Knee_R = nullptr;
	USceneComponent* Foot_R = nullptr;

	USceneComponent* UpperLeg_L = nullptr;
	USceneComponent* Knee_L = nullptr;
	USceneComponent* Foot_L = nullptr;

	//Positions
	FVector Pos_Hips = FVector::ZeroVector;

	FVector Pos_UpperLeg_R = FVector::ZeroVector;
	FVector Pos_Knee_R = FVector::ZeroVector;
	FVector Pos_Foot_R = FVector::ZeroVector;

	FVector Pos_UpperLeg_L = FVector::ZeroVector;
	FVector Pos_Knee_L = FVector::ZeroVector;
	FVector Pos_Foot_L = FVector::ZeroVector;
};
