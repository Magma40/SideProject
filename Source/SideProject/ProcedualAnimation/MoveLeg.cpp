// Fill out your copyright notice in the Description page of Project Settings.

#include "ProcedualAnimation/MoveLeg.h"
#include "Tools/GenericCalls.h"

UMoveLeg::UMoveLeg() { PrimaryComponentTick.bCanEverTick = true; }

void UMoveLeg::Init()
{
	LegParts.Empty();
	OtherLegs.Empty();
	LegParts.Add(this);

	// Get Knee
	LegParts.Add(GenericCalls::FindChildByPrefix(this, TEXT("Knee_")));
	if (!IsValid(LegParts.Last()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - Knee_ not found or wrong type"),  *StaticClass()->GetName());
		return;
	}

	// Get Foot
	LegParts.Add(GenericCalls::FindChildByPrefix(LegParts.Last(), TEXT("Foot_")));
	if (!IsValid(LegParts.Last()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - Foot_ not found or wrong type"),  *StaticClass()->GetName());
		return;
	}

	// Get Other Leg
	TArray<USceneComponent*> OtherComponents = GenericCalls::FindChildrenByPrefix(GetOwner(), TEXT("UpperLeg_"));
	for (USceneComponent* Child : OtherComponents)
	{
		if (UMoveLeg* Other = Cast<UMoveLeg>(Child))
		{
			if (Other != this) OtherLegs.Add(Other);
		}
	}
}

void UMoveLeg::BeginPlay() // Please come home, because I miss you Gary!!
{
	Super::BeginPlay();
	Init();
}

void UMoveLeg::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateIK(DeltaTime);
}

//Update the step animation
void UMoveLeg::UpdateStepping(const float DeltaTime)
{
	//Move from A to B this current frame
	StepElapsed += DeltaTime;
	const float Alpha = FMath::Clamp(StepElapsed / StepTime, 0.f, 1.f);
	LiftAmount = FMath::Sin(Alpha * PI) * StepHeight;
	
	TargetPose.Position = FMath::Lerp(SourcePose.Position, StepTargetPose.Position, Alpha) + FVector(0, 0, LiftAmount);
	TargetPose.Rotation = FQuat::Slerp(SourcePose.Rotation, StepTargetPose.Rotation, Alpha);
	TargetLocal = TargetPose.Position;

	//If leg has completed a step, reset animation 
	if (Alpha >= 1.f - KINDA_SMALL_NUMBER) // wtf?
	{
		bIsStepping = false;
		bFootPlanted = true;
		LastStepEndTime = GetWorld()->TimeSeconds;

		LockedFootPosition.Position = LegParts[0]->GetComponentTransform().TransformPosition(TargetPose.Position);
		LiftAmount = 0.f;
	}
}

//Checks if the leg is able to step and wont affect anyhting else 
bool UMoveLeg::ShouldStep() const
{
	if (bIsJumping || bIsStepping) return false;
	if (GetWorld()->TimeSeconds - LastStepEndTime < StepCooldown) return false;

	//If there is enough distance to perform a nice step animation
	const FVector LocalLockedFootPos = LegParts[0]->GetComponentTransform().InverseTransformPosition(LockedFootPosition.Position);
	const float Dist = FVector::Dist(GetGroundPose().Position, LocalLockedFootPos);
	if (Dist < StepTrigger) return false;

	//If other legs are stepping, this leg should wait
	for (const UMoveLeg* L : OtherLegs)
	{
		if (L && L->IsStepping())
		{
			return false;
		}
	}
	return true;
}

void UMoveLeg::StartStep() //Reset Step variables
{
	bIsStepping = true;
	StepElapsed = 0.0f;
	SourcePose.Position = LegParts[0]->GetComponentTransform().InverseTransformPosition(LegParts[2]->GetComponentLocation());
	SourcePose.Rotation = LegParts[2]->GetComponentQuat();
	StepTargetPose = GetGroundPose();// where it SHOULD move
}

void UMoveLeg::UpdateIK(float DeltaTime)
{
	if (LegParts.Num() != 3) return;
	if (!IsValid(LegParts[0]) || !IsValid(LegParts[1]) || !IsValid(LegParts[2])) return;
	if (bIsStepping) UpdateStepping(DeltaTime);
	
	USceneComponent* UpperLeg = LegParts[0];
	USceneComponent* Knee = LegParts[1];
	USceneComponent* Foot = LegParts[2];

	//Setup offset between left and right leg
	if (UpperLeg->GetName().EndsWith(TEXT("_R"))) UpperLeg->SetRelativeLocation(FVector(20, 0, -10));
	else UpperLeg->SetRelativeLocation(FVector(-20, 0, -10));
	
	Knee->SetRelativeLocation(FVector(0, 0, -50)); 
	Foot->SetRelativeLocation(FVector(0, 0, -50)); 
	
	//Get each leg parts position relative to the UpperLeg
	const FTransform  UpperLegXform = UpperLeg->GetComponentTransform();
	FVector UpperLegPos = FVector::ZeroVector;
	FVector KneePos     = UpperLegXform.InverseTransformPosition(Knee->GetComponentLocation());
	FVector FootPos     = UpperLegXform.InverseTransformPosition(Foot->GetComponentLocation());
	FVector TargetPos = TargetLocal;
	
	// --- TWO BONE IK ---
	FVector RootToEff = TargetPos - UpperLegPos;
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

	//Assemble a new position where knee should be
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle), PlaneNormal);
	FVector NewKneePos = UpperLegPos + RotatedDir * UpperLen;

	//Make sure UpperLegLimb stays the same length 
	FVector UpperLegToKnee = (NewKneePos - UpperLegPos).GetSafeNormal() * UpperLen;
	FVector FixedKneePos = UpperLegPos + UpperLegToKnee;

	//Make sure LowerLegLimb stays the same length
	FVector KneeToFoot = (TargetPos - FixedKneePos).GetSafeNormal() * LowerLen;
	FVector FixedFootPos = FixedKneePos + KneeToFoot;

	//Apply new animation locations
	Knee->SetRelativeLocation(FixedKneePos);
	FVector FootWorldPos = UpperLeg->GetComponentTransform().TransformPosition(FixedFootPos);
	FVector FootLocalToKnee = Knee->GetComponentTransform().InverseTransformPosition(FootWorldPos);
	Foot->SetRelativeLocation(FootLocalToKnee);
}

//Try to find height difference to ground
FLegsPose UMoveLeg::GetGroundPose() const
{
	FTransform UpperLegWorld = GetComponentTransform();
	FLegsPose Pose = TargetPose;
	FVector StartWorld = UpperLegWorld.GetLocation() + FVector(0,0,50.f);
	FVector EndWorld   = UpperLegWorld.GetLocation() + FVector(0, 0, -200.f) + LegParts[0]->GetRightVector() * FootRangeStepForward;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	//If leg is able to extend and reach the ground, else leg is flying
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		FVector LocalPoint = UpperLegWorld.InverseTransformPosition(Hit.ImpactPoint + Hit.ImpactNormal * GroundOffset);
		Pose.Position = LocalPoint;

		FVector UpLocal = UpperLegWorld.InverseTransformVector(FVector::UpVector);
		FVector NormalLocal = UpperLegWorld.InverseTransformVector(Hit.ImpactNormal);

		Pose.Rotation = FQuat::FindBetweenNormals(UpLocal, NormalLocal);
	}
	return Pose;
}