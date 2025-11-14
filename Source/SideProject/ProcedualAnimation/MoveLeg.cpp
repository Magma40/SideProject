// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveLeg.h"

UMoveLeg::UMoveLeg() { PrimaryComponentTick.bCanEverTick = true; }


void UMoveLeg::Init()
{
	LegParts.Empty();
	OtherLegs.Empty();
	LegParts.Add(this);

	TArray<USceneComponent*> LegChildren; // Get Knees
	this->GetChildrenComponents(false, LegChildren);
	for (USceneComponent* Child : LegChildren)
	{
		if (Child->GetName().Contains(TEXT("Knee_"))) LegParts.Add(Child);
	}

	TArray<USceneComponent*> KneeChildren; // Get Feet
	LegParts[1]->GetChildrenComponents(false, KneeChildren);
	for (USceneComponent* Child : KneeChildren)
	{
		if (Child->GetName().Contains(TEXT("Foot_"))) LegParts.Add(Child);
	}

	// Initialize TargetPose to current foot world position
	if (LegParts.Num() >= 3)
	{
		FVector FootWorld = LegParts[2]->GetComponentLocation();
		TargetPose.Position = FootWorld;
		TargetPose.Rotation = LegParts[2]->GetComponentQuat();
		CurrentPose = TargetPose;
		LocalGroundPosition = LegParts[2]->GetRelativeLocation();
	}
	
	USceneComponent* Parent = GetAttachParent(); // Get Other Leg
	if (!IsValid(Parent)) return;
	TArray<USceneComponent*> Siblings;
	Parent->GetChildrenComponents(false, Siblings);
	for (USceneComponent* Child : Siblings)
	{
		UMoveLeg* Other = Cast<UMoveLeg>(Child);
		// Skip myself
		if (Other && Other != this)  OtherLegs.Add(Other);
	}
}
void UMoveLeg::BeginPlay()
{
	Super::BeginPlay();
	Init();
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
	USceneComponent* UpperLeg = LegParts[0];
	USceneComponent* Knee = LegParts[1];
	USceneComponent* Foot = LegParts[2];

	FVector UpperLegPos = UpperLeg->GetComponentLocation();
	FVector KneePos = Knee->GetComponentLocation();
	FVector FootPos = Foot->GetComponentLocation();
	FVector Target = CurrentPose.Position;
	
	// --- TWO BONE IK ---
	FVector RootToEff = Target - UpperLegPos;
	float UpperLen = (KneePos - UpperLegPos).Size();
	float LowerLen = (FootPos - KneePos).Size();
	float TargetLen = RootToEff.Size();
	TargetLen = FMath::Clamp(TargetLen, FMath::Abs(UpperLen - LowerLen) + 1e-3f, UpperLen + LowerLen - 1e-3f);
	
	// Law of cosines for knee angle
	float A = (UpperLen * UpperLen + TargetLen * TargetLen - LowerLen * LowerLen) / (2.0f * UpperLen * TargetLen);
	A = FMath::Clamp(A, -1.f, 1.f);
	float KneeAngle = FMath::Acos(A);

	FVector ForwardPoleDir = (UpperLeg->GetRightVector() - UpperLeg->GetUpVector()).GetSafeNormal();
	FVector DirToTarget = RootToEff.GetSafeNormal();
	FVector PlaneNormal = FVector::CrossProduct(DirToTarget,  ForwardPoleDir).GetSafeNormal();
	if (PlaneNormal.IsNearlyZero()) PlaneNormal = FVector::UpVector;
	
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle), PlaneNormal);
	FVector NewKneePos = UpperLegPos + RotatedDir * UpperLen;

	Knee->SetWorldLocation(NewKneePos);
	Foot->SetWorldLocation(Target);
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
	SourcePose = CurrentPose;
	TargetPose = GetGroundPose();

	// Add a per-leg offset so legs don't land on the exact same spot
	FVector LegOffset = FVector::ZeroVector;
//	if (GetName().Contains(TEXT("UpperLeg_L"))) LegOffset = FVector(-20.0f, 0.0f, 0.0f);   // left leg offset
	//else if (GetName().Contains(TEXT("UpperLeg_R"))) LegOffset = FVector(20.0f, 0.0f, 0.0f); // right leg offset

	TargetPose.Position += LegOffset;
}
void UMoveLeg::UpdateStep(float DeltaTime)
{
	StepElapsed += DeltaTime;
	float Alpha = FMath::Clamp(StepElapsed / StepTime, 0.f, 1.f);
	LiftAmount = FMath::Sin(Alpha * PI) * StepHeight;

	FVector Pos = FMath::Lerp(SourcePose.Position, TargetPose.Position, Alpha) + FVector::UpVector * LiftAmount;
	FQuat Rot = FQuat::Slerp(SourcePose.Rotation, TargetPose.Rotation, Alpha);

	CurrentPose.Position = Pos;
	CurrentPose.Rotation = Rot;

	if (Alpha >= 1.f)
	{
		bIsStepping = false;
		LiftAmount = 0.f;
		CurrentPose = TargetPose;
	}
}

FLegsPose UMoveLeg::GetGroundPose() const
{
	FLegsPose Pose;
	FVector FootWorld = LegParts[2]->GetComponentLocation();
	FVector StartWorld = FootWorld + FVector(0, 0, 50.f);
	FVector EndWorld   = FootWorld + FVector(0, 0, -100.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		Pose.Position = Hit.ImpactPoint + Hit.ImpactNormal * GroundOffset;
		Pose.Rotation = FQuat::FindBetweenNormals(FVector::UpVector, Hit.ImpactNormal);
	}
	else
	{
		Pose.Position = FootWorld;
		Pose.Rotation = FQuat::Identity;
	}
	DrawDebugSphere(GetWorld(), Pose.Position, 	8.f, 12, FColor::Red);
	return Pose;
}

FVector UMoveLeg::FootBottom() const
{
	if (LegParts.Num() < 3 || !LegParts[2]) return FVector::ZeroVector;
	return LegParts[2]->GetComponentLocation() + FVector::DownVector * GroundOffset;
}

