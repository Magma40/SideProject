// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveLeg.h"

UMoveLeg::UMoveLeg() { PrimaryComponentTick.bCanEverTick = true; }

void UMoveLeg::BeginPlay()
{
	Super::BeginPlay();
	LegParts.Empty();
	LegParts.Add(this);
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);
	if (GetName().Contains(TEXT("UpperLeg_L")))
	{
		for (USceneComponent* Child : Children)
		{
			LegOffset = FVector(-20.0f, 0.0f, 0.f);
			if (Child->GetName() == TEXT("Knee_L")) LegParts.Add(Child);
			if (Child->GetName() == TEXT("Foot_L")) LegParts.Add(Child);
		}	
	}
	else
	{
		if (GetName().Contains(TEXT("UpperLeg_R")))
		{
			for (USceneComponent* Child : Children)
			{
				LegOffset = FVector(20.0f, 0.0f, 0.f);
				if (Child->GetName() == TEXT("Knee_R")) LegParts.Add(Child);
				if (Child->GetName() == TEXT("Foot_R")) LegParts.Add(Child);
			}	
		}
	}
}

void UMoveLeg::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMoveLeg::UpdateLeg()
{
	if (LegParts.Num() != 3) return;
	if (!IsValid(LegParts[0]) || !IsValid(LegParts[1]) || !IsValid(LegParts[2])) return;
	TargetPose = GetGroundPose();
	TargetPose.Position += LegOffset;
	USceneComponent* Hip = LegParts[0];
	USceneComponent* Knee = LegParts[1];
	USceneComponent* Foot = LegParts[2];

	FVector HipPos = Hip->GetComponentLocation();
	FVector KneePos = Knee->GetComponentLocation();
	FVector FootPos = Foot->GetComponentLocation();
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

	Knee->SetWorldLocation(NewKneePos);
	Foot->SetWorldLocation(Target);
}

FLegsPose UMoveLeg::GetGroundPose() const
{
	FVector Start = GetOwner()->GetActorTransform().TransformPosition(LocalGroundPosition + FVector(0, 0, 50.f));
	FVector End = Start + FVector(0, 0, -100.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FLegsPose Pose;
	Pose.Position = End;
	Pose.Rotation = FQuat::Identity;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		Pose.Position = Hit.ImpactPoint + Hit.ImpactNormal * GroundOffset;
		Pose.Rotation = FQuat::FindBetweenNormals(FVector::UpVector, Hit.ImpactNormal);
	}

	return Pose;
}

FVector UMoveLeg::FootBottom() const
{
	if (LegParts.Num() < 3 || !LegParts[2]) return FVector::ZeroVector;
	return LegParts[2]->GetComponentLocation() + FVector::DownVector * GroundOffset;
}

