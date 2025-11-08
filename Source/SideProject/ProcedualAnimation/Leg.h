// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Leg.generated.h"

USTRUCT(BlueprintType)
struct FLegPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API ULeg : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	ULeg();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	TArray<USceneComponent*> Bones; // 0=Hip, 1=Knee, 2=Foot

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	TArray<FVector> BoneOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	float StepTrigger = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	float StepTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	float StepHeight = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	float GroundOffset = 0.5f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsStepping = false;

	UPROPERTY(BlueprintReadOnly)
	float LiftAmount = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FLegPose TargetPose;

	UPROPERTY(BlueprintReadOnly)
	FVector Pole;

	// Functions
	FLegPose GetGroundPose() const;
	void UpdateLeg(float DeltaTime);
	bool ShouldStep() const;
	void DoStep(const FLegPose& Source, const FLegPose& Target);

private:
	FLegPose CurrentGroundPose;
	FVector LocalGroundPosition;
	FTimerHandle StepTimerHandle;
};
