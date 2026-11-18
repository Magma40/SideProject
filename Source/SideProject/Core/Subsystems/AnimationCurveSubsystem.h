//This script is not mine: https://github.com/tomlooman/ActionRoguelike/blob/master/Source/ActionRoguelike/Animation/RogueCurveAnimSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/WorldSubsystem.h"
#include "Curves/CurveVector.h"
#include "AnimationCurveSubsystem.generated.h"

USTRUCT()
struct FActiveCurveAnim
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	FActiveCurveAnim() {}

	FActiveCurveAnim(UCurveFloat* InCurve, TFunction<void (float)> InCallback, float InRate = 1.0f)
	{
		Curve = InCurve;
		Callback = InCallback;
		PlayRate = InRate;

		// We use first keyframe as start time, you could choose to always start at 0.0f
		Curve->GetTimeRange(CurrentTime, MaxTime);
	}
	
	void Tick(const float DeltaTime)
	{
		CurrentTime += (DeltaTime*PlayRate);
		
		float CurrentValue = Curve->GetFloatValue(CurrentTime);
		Callback(CurrentValue);
		
		if (CurrentTime >= MaxTime)
		{
			//Mark as "Finished", will be cleaned up by subsystem
			Curve = nullptr;
		}
	}

	bool IsFinished() const
	{
		return Curve == nullptr;
	}

	//FIELDS:	
public:
	UPROPERTY()
	UCurveFloat* Curve = nullptr;
	
	float PlayRate = 1.0f;
	
	float CurrentTime = 0.0f; /* Current time along curve */
	
	float MaxTime = 0.0f; 	/* Cached max time to know when we finished */

	TFunction<void(float)> Callback;
};

USTRUCT()
struct FActiveVectorCurveAnim
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	FActiveVectorCurveAnim() {}

	FActiveVectorCurveAnim(UCurveVector* InCurve,TFunction<void(const FVector&)> InCallback, float InRate = 1.0f)
	{
		Curve = InCurve;
		Callback = InCallback;
		PlayRate = InRate;

		//Cache time range
		if (Curve)
		{
			Curve->GetTimeRange(CurrentTime, MaxTime);
		}
	}
	
	void Tick(const float DeltaTime)
	{
		if (!Curve)
		{
			return;
		}

		CurrentTime += DeltaTime * PlayRate;

		const FVector CurrentValue = Curve->GetVectorValue(CurrentTime);
		Callback(CurrentValue);

		if (CurrentTime >= MaxTime)
		{
			Curve = nullptr; //Mark finished
		}
	}

	bool IsFinished() const
	{
		return Curve == nullptr;
	}

	//FIELDS:
public:

	UPROPERTY() 
	UCurveVector* Curve = nullptr;

	float PlayRate = 1.0f;
	
	float CurrentTime = 0.0f;
	
	float MaxTime = 0.0f;

	TFunction<void(const FVector&)> Callback;
};


USTRUCT()
struct FActiveEasingFunc
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	
	FActiveEasingFunc() {}

	FActiveEasingFunc(float InEasingExp, float InPlayRate, TFunction<void (float)> InCallback) : EasingExp(InEasingExp), PlayRate(InPlayRate), Callback(InCallback) 	{}
	
	void Tick(const float DeltaTime)
	{
		Time += (DeltaTime*PlayRate);

		//Clamp
		Time = FMath::Min(Time, 1.0f);

		const float CurrentValue = FMath::InterpEaseInOut(0.0f, 1.0f, Time, EasingExp);

		Callback(CurrentValue);
	}

	bool IsFinished() const 
	{
		return Time >= 1.0f;;
	}
	
	//FIELDS:	
public:
	float EasingExp = 2.0f;
	
	float PlayRate = 1.0f;
	
	float Time = 0.0f;

	TFunction<void(float)> Callback;
};

UCLASS()
class SIDEPROJECT_API UAnimationCurveSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
	//FUNCTIONS:	
public:
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;
	
	/* Start animation based on curve */
	void PlayCurveAnim(UCurveFloat* InCurveAsset, 
		float InPlayRate, 
		const TFunction<void(float)>& Func);

	void PlayCurveAnim(UCurveVector* InCurveAsset, 
		float InPlayRate, 
		const TFunction<void(const FVector&)>& Func);

	/* Play easing function with output between 0.0 - 1.0 */
	void PlayEasingFunc(EEasingFunc::Type EasingType, 
		float EasingExp,
		float InPlayRate, 
		const TFunction<void(float)>& Func);

	static TTuple<TTuple<TArray<FTransform>, float>,FTransform> CustomAnim(float DeltaTime, 
		TArray<FTransform> TheTransforms, 
		float ElapsedTime, 
		float PlayRate);
	
	static TArray<FVector> GenerateInBetweenAnimPositions(TArray<FVector> TheLocations, int32 HowManyIndexes);
	
	//FIELDS:
public:
	UPROPERTY()	
	TArray<FActiveCurveAnim> ActiveAnims;
	
	UPROPERTY()	
	TArray<FActiveVectorCurveAnim> ActiveVectorAnims;
	
	UPROPERTY()	
	TArray<FActiveEasingFunc> ActiveEasingFuncs;
};
