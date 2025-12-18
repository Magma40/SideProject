// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveArm.h"

UMoveArm::UMoveArm() { PrimaryComponentTick.bCanEverTick = true; }

void UMoveArm::Init()
{
	ArmParts.Empty();
	OtherArms.Empty();
	ArmParts.Add(this);

	TArray<USceneComponent*> ArmChildren; // Get Elbows
	this->GetChildrenComponents(false, ArmChildren);
	for (USceneComponent* Child : ArmChildren)
	{
		if (Child->GetName().Contains(TEXT("Elbow_"))) ArmParts.Add(Child);
	}

	TArray<USceneComponent*> ElbowChildren; // Get Hands
	ArmParts[1]->GetChildrenComponents(false, ElbowChildren);
	for (USceneComponent* Child : ElbowChildren)
	{
		if (Child->GetName().Contains(TEXT("Hand_"))) ArmParts.Add(Child);
	}
	
	USceneComponent* Parent = GetAttachParent(); // Get Other Arm
	if (!IsValid(Parent)) return;
	TArray<USceneComponent*> Siblings;
	Parent->GetChildrenComponents(false, Siblings);
	for (USceneComponent* Child : Siblings)
	{
		UMoveArm* Other = Cast<UMoveArm>(Child);
		// Skip myself
		if (Other && Other != this)  OtherArms.Add(Other);
	}
}

void UMoveArm::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

// Called every frame
void UMoveArm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMoveArm::SwitchState(EAnimState NewState)
{
	if (State == NewState) return;
	State = NewState;
	switch (State)
	{
	case EAnimState::Idle:		EnterIdle(); break;
	case EAnimState::Moving:	EnterMoving(); break;
	case EAnimState::Jumping:	EnterJumping(); break;
	case EAnimState::Landing:	EnterLanding(); break;
	case EAnimState::None :
		{
			UE_LOG(LogTemp, Error, TEXT("State Is None"));
			SwitchState(EAnimState::Idle);
			break;
		}
	}
}

void UMoveArm::UpdateState(float DeltaTime)
{
	switch (State)
	{
	case EAnimState::Idle:		UpdateIdle(DeltaTime); break;
	case EAnimState::Moving:	UpdateMoving(DeltaTime); break;
	case EAnimState::Jumping:	UpdateJumping(DeltaTime); break;
	case EAnimState::Landing:	UpdateLanding(DeltaTime); break;
	case EAnimState::None :
		{
			UE_LOG(LogTemp, Error, TEXT("State Is None"));
			SwitchState(EAnimState::Idle);
			break;
		}
	}
}

void UMoveArm::SetIdleAnim() { SwitchState(EAnimState::Idle); }

void UMoveArm::SetMovingAnim() {SwitchState(EAnimState::Moving);}

void UMoveArm::SetJumpingAnim() { SwitchState(EAnimState::Jumping);}

void UMoveArm::EnterIdle()
{
	ActionEnabled = false;
	if (ShouldHoldUpHands()) return;
	TargetPose.Position = FVector(0,25,  -50);
	TargetPose.Rotation = FQuat::Identity;
}

void UMoveArm::EnterMoving()
{

}

void UMoveArm::EnterJumping()
{
}

void UMoveArm::EnterLanding()
{
}

void UMoveArm::UpdateIdle(float DeltaTime)
{
	// Nothing special here (foot locked)
}

void UMoveArm::UpdateMoving(float DeltaTime)
{
}

void UMoveArm::UpdateJumping(float DeltaTime)
{

}

void UMoveArm::UpdateLanding(float DeltaTime)
{
	// Optional landing smoothing
}

void UMoveArm::SetWaveAnim()
{
		ActionEnabled = true;
		TargetPose.Position = FVector(20,0,  50);
		TargetPose.Rotation = FQuat::Identity;	
}

void UMoveArm::SetBackHolsterAnim(FVector HolsterLocation)
{
	ActionEnabled = true;
	const FTransform Xform = ArmParts[0]->GetComponentTransform();
	FVector HolsterLocationPos = Xform.InverseTransformPosition(HolsterLocation);
	TargetPose.Position = HolsterLocationPos;
	TargetPose.Rotation = FQuat::Identity;	
}

void UMoveArm::UpdateArm()
{
	if (ArmParts.Num() != 3) return;
	if (!IsValid(ArmParts[0]) || !IsValid(ArmParts[1]) || !IsValid(ArmParts[2])) return;
	USceneComponent* UpperArm = ArmParts[0];
	USceneComponent* Elbow = ArmParts[1];
	USceneComponent* Hand= ArmParts[2];

	const FTransform Xform = UpperArm->GetComponentTransform();
	FVector UpperArmPos = Xform.InverseTransformPosition(UpperArm->GetComponentLocation());
	FVector ElbowPos     = Xform.InverseTransformPosition(Elbow->GetComponentLocation());
	FVector HandPos     = Xform.InverseTransformPosition(Hand->GetComponentLocation());
	FVector Target = TargetPose.Position;
	if (!ActionEnabled) 	SetIdleAnim();

	// --- TWO BONE IK ---
	FVector RootToEff = Target - UpperArmPos;
	float UpperLen = (ElbowPos - UpperArmPos).Size();
	float LowerLen = (HandPos - ElbowPos).Size();
	float TargetLen = RootToEff.Size();
	TargetLen = FMath::Clamp(TargetLen, FMath::Abs(UpperLen - LowerLen) + 1e-3f, UpperLen + LowerLen - 1e-3f);
	
	// Law of cosines for knee angle
	float A = (UpperLen * UpperLen + TargetLen * TargetLen - LowerLen * LowerLen) / (2.0f * UpperLen * TargetLen);
	A = FMath::Clamp(A, -1.f, 1.f);
	float ElbowAngle = FMath::Acos(A);

	FVector ForwardPoleDirWS = -UpperArm->GetRightVector() - UpperArm->GetUpVector();
	FVector ForwardPoleDir = Xform.InverseTransformVector(ForwardPoleDirWS).GetSafeNormal();
	if (ForwardPoleDir.IsNearlyZero()) ForwardPoleDir = FVector(0, 1, 0); 
	
	FVector DirToTarget = RootToEff.GetSafeNormal();
	FVector PlaneNormal = FVector::CrossProduct(DirToTarget,  ForwardPoleDir).GetSafeNormal();
	if (PlaneNormal.IsNearlyZero()) PlaneNormal = FVector::UpVector;
	
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(ElbowAngle), PlaneNormal);
	FVector NewElbowPos = UpperArmPos + RotatedDir * UpperLen;

	FVector UpperArmToElbow = (NewElbowPos - UpperArmPos).GetSafeNormal() * UpperLen;
	FVector FixedElbowPos = UpperArmPos + UpperArmToElbow;

	FVector ElbowToTargetHand = (Target - FixedElbowPos).GetSafeNormal() * LowerLen;
	FVector FixedHandPos = FixedElbowPos + ElbowToTargetHand;
	
	Elbow->SetRelativeLocation(FixedElbowPos);
	FVector HandWorldPos = UpperArm->GetComponentTransform().TransformPosition(FixedHandPos);
	FVector HandLocalToElbow = Elbow->GetComponentTransform().InverseTransformPosition(HandWorldPos);
	Hand->SetRelativeLocation(HandLocalToElbow);
}

bool UMoveArm::ShouldHoldUpHands()
{
	FTransform UpperArmWorld = this->GetComponentTransform();
	FVector StartWorld = UpperArmWorld.GetLocation();
	FVector EndWorld   = UpperArmWorld.GetLocation()  + GetRightVector() * 75;
	TargetPose.Position = FVector(0,25,  -50);
	TargetPose.Rotation = FQuat::Identity;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()->GetParentActor());
	Params.AddIgnoredActor(GetOwner());
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		FVector LocalPoint = UpperArmWorld.InverseTransformPosition(Hit.ImpactPoint);
		TargetPose.Position = LocalPoint;

		FVector UpLocal = UpperArmWorld.InverseTransformVector(FVector::UpVector);
		FVector NormalLocal = UpperArmWorld.InverseTransformVector(Hit.ImpactNormal);

		TargetPose.Rotation = FQuat::FindBetweenNormals(UpLocal, NormalLocal);
		return true;
	}
	FVector WorldPos = UpperArmWorld.TransformPosition(TargetPose.Position);
	//DrawDebugLine(GetWorld(), StartWorld, EndWorld, FColor::Red);
	//DrawDebugSphere(GetWorld(), WorldPos, 	8.f, 12, FColor::Red);
	return false;
}

