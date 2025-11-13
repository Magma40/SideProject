// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveLeg.h"

UMoveLeg::UMoveLeg() { PrimaryComponentTick.bCanEverTick = true; }

void UMoveLeg::BeginPlay()
{
	Super::BeginPlay();
	LegParts.Empty();
	OtherLegs.Empty();
	LegParts.Add(this);
	TArray<USceneComponent*> Children;
	GetAttachmentRoot()->GetChildrenComponents(true, Children);
	if (GetName().Contains(TEXT("UpperLeg_L")))
	{
		for (USceneComponent* Child : Children)
		{
			if (Child->GetName() == TEXT("Knee_L")) LegParts.Add(Child);
			if (Child->GetName() == TEXT("Foot_L")) LegParts.Add(Child);
			if (Child->GetName() == TEXT("UpperLeg_R")) OtherLegs.Add(Cast<UMoveLeg>(Child));
		}	
	}
	else
	{
		if (GetName().Contains(TEXT("UpperLeg_R")))
		{
			for (USceneComponent* Child : Children)
			{
				if (Child->GetName() == TEXT("Knee_R")) LegParts.Add(Child);
				if (Child->GetName() == TEXT("Foot_R")) LegParts.Add(Child);
				if (Child->GetName() == TEXT("UpperLeg_L")) OtherLegs.Add(Cast<UMoveLeg>(Child));
			}	
		}
	}
}

void UMoveLeg::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsStepping && ShouldStep()) StartStep();
	if (bIsStepping)  UpdateStep(DeltaTime);
}

void UMoveLeg::UpdateLeg()
{
	if (LegParts.Num() != 3) return;
	if (!IsValid(LegParts[0]) || !IsValid(LegParts[1]) || !IsValid(LegParts[2])) return;
	USceneComponent* Hip = LegParts[0];
	USceneComponent* Knee = LegParts[1];
	USceneComponent* Foot = LegParts[2];

	FVector HipPos = Hip->GetRelativeLocation();
	FVector KneePos = Knee->GetRelativeLocation();
	FVector FootPos = Foot->GetRelativeLocation();
	FVector Target = TargetPose.Position;
	
	// --- TWO BONE IK ---
	FVector RootToEff = Target - HipPos;
	float UpperLen = (KneePos - HipPos).Size();
	float LowerLen = (FootPos - KneePos).Size();
	float TargetLen = RootToEff.Size();
	TargetLen = FMath::Clamp(TargetLen, FMath::Abs(UpperLen - LowerLen) + 1e-3f, UpperLen + LowerLen - 1e-3f);
	
	// Law of cosines for knee angle
	float A = (UpperLen * UpperLen + TargetLen * TargetLen - LowerLen * LowerLen) / (2.0f * UpperLen * TargetLen);
	A = FMath::Clamp(A, -1.f, 1.f);
	float KneeAngle = FMath::Acos(A);

	FVector ForwardPoleDir = (Hip->GetRightVector() - Hip->GetUpVector()).GetSafeNormal();
	FVector DirToTarget = (Target - HipPos).GetSafeNormal();
	FVector PlaneNormal = FVector::CrossProduct(DirToTarget, ForwardPoleDir).GetSafeNormal();
	if (PlaneNormal.IsNearlyZero()) PlaneNormal = FVector::UpVector;
	
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle), PlaneNormal);
	FVector NewKneePos = HipPos + RotatedDir * UpperLen;

	Knee->SetRelativeLocation(NewKneePos);
	Foot->SetRelativeLocation(Target);
}

bool UMoveLeg::ShouldStep() const
{
	float Dist = FVector::Dist(TargetPose.Position, GetGroundPose().Position);
	return Dist > StepTrigger && !bIsStepping;
	// float Dist = FVector::Dist(TargetPose.Position, GetGroundPose().Position);
	// if (Dist < StepTrigger) return false;
	//
	// int32 SteppingLegs = 0;
	// for (UMoveLeg* Leg : OtherLegs)
	// {
	// 	if (Leg && Leg->IsStepping()) SteppingLegs++;
	// }
	//
	// if (SteppingLegs > 0) return false;
	// return true;
}
void UMoveLeg::StartStep()
{
	bIsStepping = true;
	StepElapsed = 0.0f;
	SourcePose = TargetPose;
	TargetPose = GetGroundPose();

	// Add a per-leg offset so legs don't land on the exact same spot
	FVector LegOffset = FVector::ZeroVector;
	if (GetName().Contains(TEXT("UpperLeg_L"))) LegOffset = FVector(-20.0f, 0.0f, 0.0f);   // left leg offset
	else if (GetName().Contains(TEXT("UpperLeg_R"))) LegOffset = FVector(20.0f, 0.0f, 0.0f); // right leg offset

	TargetPose.Position += LegOffset;
}
void UMoveLeg::UpdateStep(float DeltaTime)
{
	StepElapsed += DeltaTime;
	float Alpha = FMath::Clamp(StepElapsed / StepTime, 0.f, 1.f);

	LiftAmount = FMath::Sin(Alpha * PI) * StepHeight;

	FVector Pos = FMath::Lerp(SourcePose.Position, TargetPose.Position, Alpha) + FVector::UpVector * LiftAmount;
	FQuat Rot = FQuat::Slerp(SourcePose.Rotation, TargetPose.Rotation, Alpha);

	TargetPose.Position = Pos;
	TargetPose.Rotation = Rot;

	if (Alpha >= 1.f)
	{
		bIsStepping = false;
		LiftAmount = 0.f;
	}
}

FLegsPose UMoveLeg::GetGroundPose() const
{
	FVector StartWorld = GetComponentTransform().TransformPosition(LocalGroundPosition + FVector(0, 0, 50.f));
	FVector EndWorld   = StartWorld + FVector(0, 0, -100.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FLegsPose Pose;
	Pose.Position = LocalGroundPosition;
	Pose.Rotation = FQuat::Identity;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		FVector LocalImpact = GetComponentTransform().InverseTransformPosition(Hit.ImpactPoint + Hit.ImpactNormal * GroundOffset);
		Pose.Position = LocalImpact;
		Pose.Rotation = FQuat::FindBetweenNormals(FVector::UpVector, Hit.ImpactNormal);
	}
	DrawDebugSphere(GetWorld(), Pose.Position, 	8.f, 12, FColor::Red);
	return Pose;
}

FVector UMoveLeg::FootBottom() const
{
	if (LegParts.Num() < 3 || !LegParts[2]) return FVector::ZeroVector;
	return LegParts[2]->GetComponentLocation() + FVector::DownVector * GroundOffset;
}

