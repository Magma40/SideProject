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

/**
 The class for the legs in procedural animation
**/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UMoveLeg : public USceneComponent
{
	GENERATED_BODY()
	
	UMoveLeg();
	
	//FUNCTIONS:
public:
	void UpdateIK(float DeltaTime);

	FLegsPose GetGroundPose() const;
	
	bool ShouldStep() const;
	void StartStep();
	void UpdateStepping(float DeltaTime);
	
	//~ Begin Getters & Setters
	FORCEINLINE const FLegsPose& GetTargetPose() const
	{
		return TargetPose;
	}
	
	FORCEINLINE bool IsStepping() const
	{
		return bIsStepping;
	}
	
	FORCEINLINE float GetLiftAmount() const
	{
		return LiftAmount;
	}
	//~ End Getters & Setters
	
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void Init();

	//FIELDS:
public:
	bool bIsStepping = false;
	
	bool bFootPlanted = true;

	bool bIsJumping = false;
	
	bool bHasLeftGround = false;
	
	float StepElapsed = 0.0f;
	
	float LiftAmount  = 0.0f;
	
	float AirElapsedTime = 0.0f;

	float LastStepEndTime = 0.0f;
	
	UPROPERTY()
	FLegsPose TargetPose = FLegsPose();
	
	UPROPERTY()
	FLegsPose SourcePose = FLegsPose();
	
	UPROPERTY()
	FLegsPose StepTargetPose  = FLegsPose();
	
	UPROPERTY()
	FLegsPose LockedFootPosition = FLegsPose();
	
	UPROPERTY()
	FVector TargetLocal = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite) 
	TArray<TObjectPtr<USceneComponent>> LegParts;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") 
	float StepTrigger = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") 
	float StepTime = 0.4f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") 
	float StepHeight = 40.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	float GroundOffset = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") 
	float FootRangeStepForward = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg IK") 
	float AirRiseSpeed = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg IK") 
	float MaxAirRise = 120.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg") 
	float StepCooldown = 0.25f;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UMoveLeg>> OtherLegs;
};
