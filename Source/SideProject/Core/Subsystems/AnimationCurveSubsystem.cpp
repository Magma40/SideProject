#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimationCurveSubsystem)

void UAnimationCurveSubsystem::Tick(const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(CurveAnimationsTick)
	
	//Curve Based Anims - Reverse to easily remove completed anims during iteration
	for (int i = ActiveAnims.Num() - 1; i >= 0; --i)
	{
		ActiveAnims[i].Tick(DeltaTime);

		if (ActiveAnims[i].IsFinished())
		{
			ActiveAnims.RemoveAtSwap(i);
		}
	}

	//Curve Based Anims - Reverse to easily remove completed anims during iteration
	for (int i = ActiveVectorAnims.Num() - 1; i >= 0; --i)
	{
		ActiveVectorAnims[i].Tick(DeltaTime);

		if (ActiveVectorAnims[i].IsFinished())
		{
			ActiveVectorAnims.RemoveAtSwap(i);
		}
	}

	//Easing Functions
	for (int i = ActiveEasingFuncs.Num() - 1; i >= 0; --i)
	{
		ActiveEasingFuncs[i].Tick(DeltaTime);

		if (ActiveEasingFuncs[i].IsFinished())
		{
			ActiveEasingFuncs.RemoveAtSwap(i);
		}
	}
}


void UAnimationCurveSubsystem::PlayCurveAnim(UCurveFloat* InCurveAsset, 
	const float InPlayRate,
	const TFunction<void (float CurrentValue)>& Func)
{
	check(InCurveAsset);

	ActiveAnims.Add(FActiveCurveAnim(InCurveAsset, Func, InPlayRate));
}

void UAnimationCurveSubsystem::PlayCurveAnim(UCurveVector* InCurveAsset, 
	const float InPlayRate, 
	const TFunction<void(const FVector&)>& Func)
{
	check(InCurveAsset);

	ActiveVectorAnims.Add(FActiveVectorCurveAnim(InCurveAsset, Func, InPlayRate));
}


void UAnimationCurveSubsystem::PlayEasingFunc(const EEasingFunc::Type EasingType,
	const float EasingExp,
	const float InPlayRate, 
	const TFunction<void(float)>& Func)
{
	// In prototype only supporting this one type...
	check(EasingType == EEasingFunc::EaseInOut);

	ActiveEasingFuncs.Add(FActiveEasingFunc(EasingExp, InPlayRate, Func));
}


TStatId UAnimationCurveSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(AnimationCurveSubsystem, STATGROUP_Tickables);
}

TTuple<TTuple<TArray<FTransform>, float>,FTransform> UAnimationCurveSubsystem::CustomAnim(const float DeltaTime, 
	TArray<FTransform> TheTransforms, 
	float ElapsedTime, 
	float PlayRate)
{
	//Try to equal out the speed based on how many position animation should iterate through
	PlayRate = PlayRate / TheTransforms.Num();
	ElapsedTime += DeltaTime;

	//Calculate new position and rotation smoothly
	const float Alpha = FMath::Clamp(ElapsedTime / PlayRate, 0.f, 1.f);
	
	const FVector NewLocation = FMath::Lerp(TheTransforms[0].GetLocation(), TheTransforms[1].GetLocation(), Alpha);
	const FQuat NewRotation = FQuat::Slerp(TheTransforms[0].GetRotation(), TheTransforms[1].GetRotation(), Alpha);
	
	FTransform NewTransform = FTransform(NewRotation, NewLocation);

	//If animation has reached an animation point, remove it and continue to next
	if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
	{
		TheTransforms.RemoveAt(0);
		ElapsedTime = 0.0f;
	}
	
	return MakeTuple(MakeTuple(TheTransforms, ElapsedTime),NewTransform);
}

TArray<FVector> UAnimationCurveSubsystem::GenerateInBetweenAnimPositions(TArray<FVector> TheLocations, const int32 HowManyIndexes)
{
	TArray<FVector> NewInBetweenAnimPositions;

	if (TheLocations.Num() < 2 || HowManyIndexes <= 0)
	{
		return NewInBetweenAnimPositions;
	}

	const FVector& Start = TheLocations[0];
	const FVector& End   = TheLocations[1];

	const float Step = 1.f / (HowManyIndexes + 1);

	for (int32 i = 1; i <= HowManyIndexes; ++i)
	{
		float Alpha = Step * i;
		FVector Point = FMath::Lerp(Start, End, Alpha);
		NewInBetweenAnimPositions.Add(Point);
	}

	NewInBetweenAnimPositions.Insert(Start, 0);
	NewInBetweenAnimPositions.Insert(End, NewInBetweenAnimPositions.Num());
	
	return NewInBetweenAnimPositions;
}