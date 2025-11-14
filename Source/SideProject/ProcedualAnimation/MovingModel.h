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
	void UpdateHips();
	void UpdateMovement(float DeltaTime);
	
	void DrawDebugJoints();
	void DrawDebugLimbs();

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
	TArray<UMoveLeg*>  Legs;
	TArray<USceneComponent*> Knees;
	TArray<USceneComponent*> Feet;
};
