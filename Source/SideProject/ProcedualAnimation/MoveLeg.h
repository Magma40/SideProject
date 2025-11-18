// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MoveLeg.generated.h"

USTRUCT(BlueprintType)
struct FLegsPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UMoveLeg : public USceneComponent
{
	GENERATED_BODY()

protected:
	UMoveLeg();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category="Leg") float StepTrigger = 2.0f;
	UPROPERTY(EditAnywhere, Category="Leg") float StepTime = 0.5f;
	UPROPERTY(EditAnywhere, Category="Leg") float StepHeight = 2.0f;
	UPROPERTY(EditAnywhere, Category="Leg") float GroundOffset = 0.5f;
	UPROPERTY() FVector LocalGroundPosition = FVector::ZeroVector;

	UPROPERTY() bool bIsStepping = false;
	UPROPERTY() float StepElapsed = 0.0f;
	UPROPERTY() float LiftAmount  = 0.0f;

	UPROPERTY() FLegsPose TargetPose = FLegsPose();
	UPROPERTY() FLegsPose SourcePose = FLegsPose();
	UPROPERTY() FLegsPose StepTargetPose  = FLegsPose();
	TArray<USceneComponent*> LegParts;
	UPROPERTY() TArray<UMoveLeg*> OtherLegs;

public:
	bool IsStepping() const { return bIsStepping; }
	bool ShouldStep() const;
	void StartStep();
	void UpdateStep(float DeltaTime);
	float GetLiftAmount() const { return LiftAmount; }
	
	const FLegsPose& GetTargetPose() const { return TargetPose; } // this was changed
	FLegsPose GetGroundPose() const;

	FVector GetFootWorldPos() const { return LegParts[2]->GetComponentLocation(); }
	void UpdateLeg();

private:
	void Init();
};
