// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveArm.h"
#include "Tools/GenericCalls.h"
#include "Kismet/GameplayStatics.h"

UMoveArm::UMoveArm() { PrimaryComponentTick.bCanEverTick = true; }

void UMoveArm::Init()
{
	ArmParts.Empty();
	ArmParts.Add(this);

	// Get Elbow
	ArmParts.Add(GenericCalls::FindChildByPrefix(this, TEXT("Elbow_")));
	if (!IsValid(ArmParts.Last()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - Elbow_ not found or wrong type"),  *StaticClass()->GetName());
		return;
	}

	// Get Hand
	ArmParts.Add(GenericCalls::FindChildByPrefix(ArmParts.Last(), TEXT("Hand_")));
	if (!IsValid(ArmParts.Last()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - Hand_ not found or wrong type"),  *StaticClass()->GetName());
		return;
	}
}

void UMoveArm::BeginPlay() //And now my heart is beating like the saddest metronome!!
{
	Super::BeginPlay();
	Init();
	TargetPose.Position = FVector(0,25,  -50);
	TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
}

void UMoveArm::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateIK();
}

void UMoveArm::UpdateIK()
{
	if (ArmParts.Num() != 3) return;
	if (!IsValid(ArmParts[0]) || !IsValid(ArmParts[1]) || !IsValid(ArmParts[2])) return;
	//if (TargetPose.Position.IsNearlyZero()) return;
	USceneComponent* UpperArm = ArmParts[0];
	USceneComponent* Elbow = ArmParts[1];
	USceneComponent* Hand= ArmParts[2];

	const FTransform Xform = UpperArm->GetComponentTransform();
	FVector UpperArmPos = Xform.InverseTransformPosition(UpperArm->GetComponentLocation());
	FVector ElbowPos     = Xform.InverseTransformPosition(Elbow->GetComponentLocation());
	FVector HandPos     = Xform.InverseTransformPosition(Hand->GetComponentLocation());
	FVector Target = TargetPose.Position;

	// Optional swing constraint, this stops arm from going into body
	if (bEnableArmConstraints)
	{
		FVector TargetWorld = Xform.TransformPosition(TargetPose.Position);
		FVector RestDirWS = (-UpperArm->GetForwardVector() + UpperArm->GetRightVector()).GetSafeNormal() ; // rest pose direction
		FVector DesiredDirWS = (TargetWorld - Xform.GetLocation()).GetSafeNormal();
		float TargetLen = (TargetWorld - Xform.GetLocation()).Size();
		
		float MaxSwingDegrees = 75.f; // tweak per character
		FVector LimitedDirWS = ConstrainSwingTwist(RestDirWS, DesiredDirWS, MaxSwingDegrees);
		TargetWorld = Xform.GetLocation() + LimitedDirWS * TargetLen;

		Target = Xform.InverseTransformPosition(TargetWorld);
	}

	// --- TWO BONE IK ---
	FVector RootToEff = Target - UpperArmPos;
	float UpperLen = (ElbowPos - UpperArmPos).Size();
	float LowerLen = (HandPos - ElbowPos).Size();
	float TargetLen  = RootToEff.Size();
	TargetLen  = FMath::Clamp(TargetLen ,FMath::Abs(UpperLen - LowerLen) + 1e-3f,UpperLen + LowerLen - 1e-3f);
	
	// Clamp arm positions so it cant extend to outside arm length
	float A = (UpperLen * UpperLen + TargetLen  * TargetLen  - LowerLen * LowerLen) / (2.0f * UpperLen * TargetLen);
	A = FMath::Clamp(A, -1.f, 1.f);
	float ElbowAngle = FMath::Acos(A);

	//Get which direction the elbow should face
	FVector PoleDirWS = -UpperArm->GetRightVector();
	FVector PoleDirLS = Xform.InverseTransformVector(PoleDirWS).GetSafeNormal();

	//Build a plane normal that defines the arm's bending plane so elbows bends consistently and does not twist unpredictably
	FVector DirToTarget = RootToEff.GetSafeNormal();
	FVector PlaneNormal = FVector::CrossProduct(DirToTarget, PoleDirLS).GetSafeNormal();
	
	//Assemble a new position where elbow and hand should be
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(ElbowAngle), PlaneNormal);
	FVector NewElbowPos = UpperArmPos + RotatedDir * UpperLen;
	FVector FixedHandPos  = NewElbowPos + (Target - NewElbowPos).GetSafeNormal() * LowerLen;

	//Apply new positions
	Elbow->SetRelativeLocation(NewElbowPos);
	FVector HandWorldPos = Xform.TransformPosition(FixedHandPos);
	FVector HandLocalToElbow = Elbow->GetComponentTransform().InverseTransformPosition(HandWorldPos);
	Hand->SetRelativeLocation(HandLocalToElbow);


	//Assemble a new rotations where elbow and hand should be
	FVector ElbowToHandLS  = (FixedHandPos  - NewElbowPos).GetSafeNormal();
	FVector PlaneNormalLS = FVector::CrossProduct(ElbowToHandLS, PoleDirLS).GetSafeNormal();
	FQuat ElbowRotation = FRotationMatrix::MakeFromXZ(ElbowToHandLS, PlaneNormalLS).ToQuat();

	//Apply new rotations
	//Elbow->SetRelativeRotation(ElbowRotation);
	Hand->SetRelativeRotation(TargetPose.Rotation);
}

