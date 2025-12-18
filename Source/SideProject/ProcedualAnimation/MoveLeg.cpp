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

	if (LegParts.Num() > 1)
	{
		TArray<USceneComponent*> KneeChildren;
		LegParts[1]->GetChildrenComponents(false, KneeChildren);
		for (USceneComponent* Child : KneeChildren)
		{
			if (Child->GetName().Contains(TEXT("Foot_"))) LegParts.Add(Child);
		}
	}

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
		if (UMoveLeg* Other = Cast<UMoveLeg>(Child))
		{
			if (Other != this) OtherLegs.Add(Other);
		}
	}
}
void UMoveLeg::BeginPlay()
{
	Super::BeginPlay();
	Init();
	TargetPose.Position = LegParts[0]->GetComponentTransform().InverseTransformPosition(LegParts[2]->GetComponentLocation());
	TargetPose.Rotation = LegParts[2]->GetComponentQuat();
	LockedFootPosition.Position = TargetPose.Position;
	LockedFootPosition.Rotation = TargetPose.Rotation;
}

void UMoveLeg::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateState(DeltaTime);
	UpdateIK(DeltaTime);
}

void UMoveLeg::SwitchState(EAnimState NewState)
{
	if (State == NewState) return;
	State = NewState;
	switch (State)
	{
	case EAnimState::Idle:		EnterIdle(); break;
	case EAnimState::Moving:	EnterMoving(); break;
	case EAnimState::Jumping:	EnterJumping(); break;
	case EAnimState::Stepping:	EnterStepping(); break;
	case EAnimState::Landing:	EnterLanding(); break;
	case EAnimState::None :
		{
			UE_LOG(LogTemp, Error, TEXT("State Is None"));
			SwitchState(EAnimState::Idle);
			break;
		}
	}
}

void UMoveLeg::UpdateState(float DeltaTime)
{
	switch (State)
	{
	case EAnimState::Idle:		UpdateIdle(DeltaTime); break;
	case EAnimState::Moving:	UpdateMoving(DeltaTime); break;
	case EAnimState::Jumping:	UpdateJumping(DeltaTime); break;
	case EAnimState::Stepping:	UpdateStepping(DeltaTime); break;
	case EAnimState::Landing:	UpdateLanding(DeltaTime); break;
	case EAnimState::None :
		{
			UE_LOG(LogTemp, Error, TEXT("State Is None"));
			SwitchState(EAnimState::Idle);
			break;
		}
	}
}

void UMoveLeg::SetIdleAnim() { SwitchState(EAnimState::Idle); }

void UMoveLeg::SetMovingAnim() {SwitchState(EAnimState::Moving);}

void UMoveLeg::SetJumpingAnim() { SwitchState(EAnimState::Jumping);}

void UMoveLeg::EnterIdle()
{
	bIsJumping = false;
	bFootPlanted = true;
	TargetLocal = TargetPose.Position;
}

void UMoveLeg::EnterMoving()
{
	bIsJumping = false;
	bFootPlanted = true;
	bIsStepping = false;
}

void UMoveLeg::EnterJumping()
{
	bIsJumping = true;
	bFootPlanted = false;
	bIsStepping = false;
	AirElapsedTime = 0;
	// Freeze the current foot
	LockedFootPosition.Position = LegParts[0]->GetComponentTransform().TransformPosition(TargetPose.Position);;
}

void UMoveLeg::EnterStepping() { }

void UMoveLeg::EnterLanding()
{
	bIsJumping = false;
	bFootPlanted = true;
}

void UMoveLeg::UpdateIdle(float DeltaTime)
{
	// Nothing special here (foot locked)
}

void UMoveLeg::UpdateMoving(float DeltaTime)
{
	if (ShouldStep())
	{
		StartStep();
	}
	else
	{
		const FTransform UpperLegXForm = LegParts[0]->GetComponentTransform();
		TargetLocal = UpperLegXForm.InverseTransformPosition(LockedFootPosition.Position);
	}
}

void UMoveLeg::UpdateJumping(float DeltaTime) // Why am I such a good programmer?
{
	AirElapsedTime += DeltaTime;
	float Rise = FMath::Min(AirElapsedTime * AirRiseSpeed, MaxAirRise);
	FVector Goal = StepTargetPose.Position + FVector(0, 0, Rise);
	// Smooth blend
	TargetLocal = FMath::VInterpTo(TargetLocal, Goal, DeltaTime, 10.f);
	LockedFootPosition.Position = LegParts[0]->GetComponentTransform().TransformPosition(TargetPose.Position);;
}

void UMoveLeg::UpdateStepping(float DeltaTime)
{
	StepElapsed += DeltaTime;
	float Alpha = FMath::Clamp(StepElapsed / StepTime, 0.f, 1.f);
	LiftAmount = FMath::Sin(Alpha * PI) * StepHeight;
	
	TargetPose.Position = FMath::Lerp(SourcePose.Position, StepTargetPose.Position, Alpha) + FVector(0, 0, LiftAmount);
	TargetPose.Rotation = FQuat::Slerp(SourcePose.Rotation, StepTargetPose.Rotation, Alpha);
	TargetLocal = TargetPose.Position;
	
	if (Alpha >= 1.f - KINDA_SMALL_NUMBER) // wtf?
	{
		bIsStepping = false;
		bFootPlanted = true;
		LastStepEndTime = GetWorld()->TimeSeconds;

		LockedFootPosition.Position = LegParts[0]->GetComponentTransform().TransformPosition(TargetPose.Position);
		LiftAmount = 0.f;
		SwitchState(EAnimState::Moving);
	}
}

void UMoveLeg::UpdateLanding(float DeltaTime)
{
	// Optional landing smoothing
}

bool UMoveLeg::ShouldStep() const
{
	if (State == EAnimState::Jumping || bIsStepping) return false;
	if (GetWorld()->TimeSeconds - LastStepEndTime < StepCooldown) return false;
	
	FVector LocalLockedFootPos = LegParts[0]->GetComponentTransform().InverseTransformPosition(LockedFootPosition.Position);
	float Dist = FVector::Dist(GetGroundPose().Position, LocalLockedFootPos);
	if (Dist < StepTrigger) return false;
	
	for (UMoveLeg* L : OtherLegs)
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
	SwitchState(EAnimState::Stepping);
}

void UMoveLeg::UpdateIK(float DeltaTime)
{
	if (LegParts.Num() != 3) return;
	if (!IsValid(LegParts[0]) || !IsValid(LegParts[1]) || !IsValid(LegParts[2])) return;
	
	USceneComponent* UpperLeg = LegParts[0];
	USceneComponent* Knee = LegParts[1];
	USceneComponent* Foot = LegParts[2];

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

	//Assemble a new position where knee should be to be
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle), PlaneNormal);
	FVector NewKneePos = UpperLegPos + RotatedDir * UpperLen;

	//Make sure UpperLegLimb stays the same length 
	FVector UpperLegToKnee = (NewKneePos - UpperLegPos).GetSafeNormal() * UpperLen;
	FVector FixedKneePos = UpperLegPos + UpperLegToKnee;

	//Make sure LowerLegLimb stays the same length
	FVector KneeToFoot = (TargetPos - FixedKneePos).GetSafeNormal() * LowerLen;
	FVector FixedFootPos = FixedKneePos + KneeToFoot;
	
	Knee->SetRelativeLocation(FixedKneePos);
	FVector FootWorldPos = UpperLeg->GetComponentTransform().TransformPosition(FixedFootPos);
	FVector FootLocalToKnee = Knee->GetComponentTransform().InverseTransformPosition(FootWorldPos);
	Foot->SetRelativeLocation(FootLocalToKnee);
}

FLegsPose UMoveLeg::GetGroundPose() const
{
	FTransform UpperLegWorld = GetComponentTransform();
	FLegsPose Pose = TargetPose;
	FVector StartWorld = UpperLegWorld.GetLocation() + FVector(0,0,50.f);
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
	return Pose;
}