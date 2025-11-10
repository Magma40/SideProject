// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/Foot.h"

UFoot::UFoot()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFoot::BeginPlay()
{
	Super::BeginPlay();
}

void UFoot::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateFoot();
}

void UFoot::UpdateFoot()
{
	// 1. Get height above ground (Y in Unity is Z in Unreal)
	float HeightAboveGround = GetComponentLocation().Z;

	// 2. Evaluate curve
	float LiftRotation = ToeBendCurve.GetRichCurveConst()->Eval(HeightAboveGround);

	FVector FootForward = GetForwardVector();

	for (USceneComponent* Toe : ToeComponents)
	{
		if (!Toe) continue;

		bool bBackToe = Toe->GetName().Contains(TEXT("Back"));

		// Unity used Vector3.Dot(forward, up/down)
		float GroundDot = FVector::DotProduct(FootForward, bBackToe ? FVector::UpVector : -FVector::UpVector);
		GroundDot = FMath::Clamp(GroundDot * 1.6f, 0.0f, 1.0f);

		float GroundRotation = GroundDot * FMath::Clamp(1.0f - HeightAboveGround, 0.0f, 1.0f) * -30.0f;

		// Apply rotation to the toe
		FRotator LocalRot = Toe->GetRelativeRotation();
		float NewPitch = FMath::Lerp(LiftRotation, GroundRotation, GroundDot);

		LocalRot.Pitch = NewPitch;
		Toe->SetRelativeRotation(LocalRot);
	}
}
