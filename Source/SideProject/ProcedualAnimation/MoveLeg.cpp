// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveLeg.h"

#include "ComponentUtils.h"

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

	// Initialize TargetPose to current LOCAL SPACE position
	if (LegParts.Num() >= 3)
	{
		FVector FootLocal = GetComponentTransform().InverseTransformPosition(LegParts[2]->GetComponentLocation());
		TargetPose.Position = FootLocal;
		TargetPose.Rotation = LegParts[2]->GetComponentQuat();
	}
	
	USceneComponent* Parent = GetAttachParent(); // Get Other Legs
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
	LockedFootPosition.Position = TargetPose.Position;
}

void UMoveLeg::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMoveLeg::UpdateLeg(bool IsJumping, float DeltaTime)
{
	if (LegParts.Num() != 3) return;
	if (!IsValid(LegParts[0]) || !IsValid(LegParts[1]) || !IsValid(LegParts[2])) return;
	USceneComponent* UpperLeg = LegParts[0];
	USceneComponent* Knee = LegParts[1];
	USceneComponent* Foot = LegParts[2];

	bool WasJumping = bIsJumping;
	bIsJumping = IsJumping;

	if (!bIsJumping && WasJumping)
	{
		LockedFootPosition.Position = UpperLeg->GetComponentTransform().TransformPosition(TargetPose.Position);
		bJustLanded  = true;
		LastLandTime = GetWorld()->GetTimeSeconds();
	}

	if (UpperLeg->GetName().EndsWith(TEXT("_R")))
	{
		UpperLeg->SetRelativeLocation(FVector(20, 0, -10));
	}
	else
	{
		UpperLeg->SetRelativeLocation(FVector(-20, 0, -10));
	}
	
	Knee->SetRelativeLocation(FVector(0, 0, -50)); 
	Foot->SetRelativeLocation(FVector(0, 0, -50)); 
	
	//Get each leg parts position relative to the UpperLeg
	const FTransform  UpperLegXform = UpperLeg->GetComponentTransform();
	FVector UpperLegPos = FVector::ZeroVector;
	FVector KneePos     = UpperLegXform.InverseTransformPosition(Knee->GetComponentLocation());
	FVector FootPos     = UpperLegXform.InverseTransformPosition(Foot->GetComponentLocation());
	FVector TargetLocal = FVector::ZeroVector;
	
	if (bIsStepping && !IsJumping) { TargetLocal = TargetPose.Position; }
	else if (IsJumping)
	{
		MaxAirRise = 60.0f;
		AirRiseSpeed = 100.0f;
		if (!WasJumping)
		{
			AirElapsedTime = 0.0f;
		}
		AirElapsedTime += DeltaTime;
		float RiseAmount = FMath::Min(AirElapsedTime * AirRiseSpeed, MaxAirRise);
		TargetLocal = TargetPose.Position +  FVector(0, 0, RiseAmount);
	}
	else { TargetLocal = UpperLegXform.InverseTransformPosition(LockedFootPosition.Position); }
	
	// --- TWO BONE IK ---
	FVector RootToEff = TargetLocal - UpperLegPos;
	float UpperLen = (KneePos - UpperLegPos).Size();
	float LowerLen = (FootPos - KneePos).Size();
	float TargetLen = RootToEff.Size();
	TargetLen = FMath::Clamp(TargetLen, FMath::Abs(UpperLen - LowerLen) + 1e-3f, UpperLen + LowerLen - 1e-3f);
	
	// Law of cosines for knee angle
	float A = (UpperLen * UpperLen + TargetLen * TargetLen - LowerLen * LowerLen) / (2.0f * UpperLen * TargetLen);
	A = FMath::Clamp(A, -1.f, 1.f);
	float KneeAngle = FMath::Acos(A);

	//Get which direction the knee should face
	FVector ForwardPoleDirWS = UpperLeg->GetRightVector() - UpperLeg->GetUpVector();
	FVector ForwardPoleDir = UpperLegXform.InverseTransformVector(ForwardPoleDirWS).GetSafeNormal();
	//If the computed direction is degenerate, use default
	if (ForwardPoleDir.IsNearlyZero()) ForwardPoleDir = FVector(0, -1, 0); 

	//Build a plane normal that defines the leg's bending plane so knee bends consistently and does not twist unpredictably
	FVector DirToTarget = RootToEff.GetSafeNormal();
	FVector PlaneNormal = FVector::CrossProduct(DirToTarget,  ForwardPoleDir).GetSafeNormal();
	//If the vectors are almost parallel, use world up as the plane normal.
	if (PlaneNormal.IsNearlyZero()) PlaneNormal = FVector::UpVector;

	//Assemble a new position where knee should be to be
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle), PlaneNormal);
	FVector NewKneePos = UpperLegPos + RotatedDir * UpperLen;

	//Make sure UpperLegLimb stays the same length 
	FVector UpperLegToKnee = (NewKneePos - UpperLegPos).GetSafeNormal() * UpperLen;
	FVector FixedKneePos = UpperLegPos + UpperLegToKnee;

	//Make sure LowerLegLimb stays the same length
	FVector KneeToFoot = (TargetLocal - FixedKneePos).GetSafeNormal() * LowerLen;
	FVector FixedFootPos = FixedKneePos + KneeToFoot;
	
	Knee->SetRelativeLocation(FixedKneePos);
	FVector FootWorldPos = UpperLeg->GetComponentTransform().TransformPosition(FixedFootPos);
	FVector FootLocalToKnee = Knee->GetComponentTransform().InverseTransformPosition(FootWorldPos);
	Foot->SetRelativeLocation(FootLocalToKnee);
}

bool UMoveLeg::ShouldStep() const
{
	if (bIsJumping) return false;
    if (bJustLanded && (GetWorld()->TimeSeconds - LastLandTime < 0.20f))   return false;
	
	FVector LocalLockedFootPos = LegParts[0]->GetComponentTransform().InverseTransformPosition(LockedFootPosition.Position);
    float Dist = FVector::Dist(GetGroundPose().Position, LocalLockedFootPos);
	if (Dist < StepTrigger) return false;
	
	for (UMoveLeg* L : OtherLegs)
	{
		if (L && L->IsStepping()) return false;	
	}
	
	return true;
}
void UMoveLeg::StartStep() //Reset Step variables
{
	bIsStepping = true;
	StepElapsed = 0.0f;
	SourcePose.Position = LegParts[0]->GetComponentTransform().InverseTransformPosition(LegParts[2]->GetComponentLocation());
	StepTargetPose = GetGroundPose();// where it SHOULD move
}

void UMoveLeg::UpdateStep(float DeltaTime )
{
	StepElapsed += DeltaTime;
	float Alpha = FMath::Clamp(StepElapsed / StepTime, 0.0f, 1.0f);
	LiftAmount = FMath::Sin(Alpha * PI) * StepHeight;
	
	TargetPose.Position  = FMath::Lerp(SourcePose.Position, StepTargetPose.Position, Alpha) +  FVector(0, 0, LiftAmount);
	TargetPose.Rotation = FQuat::Slerp(SourcePose.Rotation, StepTargetPose.Rotation, Alpha);

	if (Alpha >= 1.0f) // If we have completed one whole step
	{
		bIsStepping = false;
		bFootPlanted = true;
		LockedFootPosition.Position = LegParts[0]->GetComponentTransform().TransformPosition(TargetPose.Position);
		LiftAmount = 0.0f;
	}
}

FLegsPose UMoveLeg::GetGroundPose() const
{
	FLegsPose Pose;
    FTransform UpperLegWorld = this->GetComponentTransform();
	FVector StartWorld = UpperLegWorld.GetLocation() + FVector(0,0,50.f) + LegParts[0]->GetRightVector() * FootRangeStepForward;
	FVector EndWorld   = UpperLegWorld.GetLocation() + FVector(0, 0, -200.f) + LegParts[0]->GetRightVector() * FootRangeStepForward;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		FVector LocalPoint = UpperLegWorld.InverseTransformPosition(Hit.ImpactPoint + Hit.ImpactNormal * GroundOffset);
		Pose.Position = LocalPoint;

		FVector UpLocal = UpperLegWorld.InverseTransformVector(FVector::UpVector);
		FVector NormalLocal = UpperLegWorld.InverseTransformVector(Hit.ImpactNormal);

		Pose.Rotation = FQuat::FindBetweenNormals(UpLocal, NormalLocal);
	}
	FVector WorldPos = UpperLegWorld.TransformPosition(Pose.Position);
	//DrawDebugSphere(GetWorld(), WorldPos, 	8.f, 12, FColor::Red);
	return Pose;
}