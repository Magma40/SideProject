// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimationCurveSubsystem)

void UAnimationCurveSubsystem::Tick(float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(CurveAnimationsTick)
	
	// Curve Based Anims - Reverse to easily remove completed anims during iteration
	for (int i = ActiveAnims.Num() - 1; i >= 0; --i)
	{
		ActiveAnims[i].Tick(DeltaTime);

		if (ActiveAnims[i].IsFinished())
		{
			ActiveAnims.RemoveAtSwap(i);
		}
	}

	// Easing Functions
	for (int i = ActiveEasingFuncs.Num() - 1; i >= 0; --i)
	{
		ActiveEasingFuncs[i].Tick(DeltaTime);

		if (ActiveEasingFuncs[i].IsFinished())
		{
			ActiveEasingFuncs.RemoveAtSwap(i);
		}
	}
}


void UAnimationCurveSubsystem::PlayCurveAnim(UCurveFloat* InCurveAsset, float InPlayRate, const TFunction<void (float CurrentValue)>& Func)
{
	check(InCurveAsset);

	ActiveAnims.Add(FActiveCurveAnim(InCurveAsset, Func, InPlayRate));
}


void UAnimationCurveSubsystem::PlayEasingFunc(EEasingFunc::Type EasingType, float EasingExp, float InPlayRate, const TFunction<void(float)>& Func)
{
	// In prototype only supporting this one type...
	check(EasingType == EEasingFunc::EaseInOut);

	ActiveEasingFuncs.Add(FActiveEasingFunc(EasingExp, InPlayRate, Func));
}


TStatId UAnimationCurveSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(RogueTweenSubsystem, STATGROUP_Tickables);
}