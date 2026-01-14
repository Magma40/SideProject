// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MoveArm.h"
#include "Tools/GenericCalls.h"
#include "Kismet/GameplayStatics.h"

UMoveArm::UMoveArm() { PrimaryComponentTick.bCanEverTick = true; }

void UMoveArm::Init()
{
	ArmParts.Empty();
	OtherArms.Empty();
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

	// Get Other Arm
	TArray<USceneComponent*> OtherComponents = GenericCalls::FindChildrenByPrefix(GetOwner(), TEXT("UpperArm_"));
	for (USceneComponent* Child : OtherComponents)
	{
		if (UMoveArm* Other = Cast<UMoveArm>(Child))
		{
			if (Other != this) OtherArms.Add(Other);
		}
	}
}

void UMoveArm::BeginPlay() //And now my heart is beating like saddest metronome!!
{
	Super::BeginPlay();
	Init();
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
	USceneComponent* UpperArm = ArmParts[0];
	USceneComponent* Elbow = ArmParts[1];
	USceneComponent* Hand= ArmParts[2];

	const FTransform Xform = UpperArm->GetComponentTransform();
	FVector UpperArmPos = Xform.InverseTransformPosition(UpperArm->GetComponentLocation());
	FVector ElbowPos     = Xform.InverseTransformPosition(Elbow->GetComponentLocation());
	FVector HandPos     = Xform.InverseTransformPosition(Hand->GetComponentLocation());
	if (TargetPose.Position.IsNearlyZero()) return;

	FVector ShoulderWorldPos = UpperArm->GetComponentLocation();
	FVector TargetWorld = Xform.TransformPosition(TargetPose.Position);

	FVector RestDirWS = (-UpperArm->GetForwardVector() + UpperArm->GetRightVector()).GetSafeNormal() ; // rest pose direction
	FVector DesiredDirWS = (TargetWorld - ShoulderWorldPos).GetSafeNormal();
	float TargetLen = (TargetWorld - ShoulderWorldPos).Size();

	if (bEnableArmConstraints)
	{
		float MaxSwingDegrees = 75.f; // tweak per character
		FVector LimitedDirWS = ConstrainSwingTwist(RestDirWS, DesiredDirWS, MaxSwingDegrees);
		TargetWorld = ShoulderWorldPos + LimitedDirWS * TargetLen;	
	}
	FVector Target = Xform.InverseTransformPosition(TargetWorld);

	// --- TWO BONE IK ---
	FVector RootToEff = Target - UpperArmPos;
	float UpperLen = (ElbowPos - UpperArmPos).Size();
	float LowerLen = (HandPos - ElbowPos).Size();
	float ClampedTargetLen = RootToEff.Size();
	ClampedTargetLen = FMath::Clamp(ClampedTargetLen,FMath::Abs(UpperLen - LowerLen) + 1e-3f,UpperLen + LowerLen - 1e-3f);
	
	// Law of cosines for knee angle
	float A = (UpperLen * UpperLen + ClampedTargetLen * ClampedTargetLen - LowerLen * LowerLen) / (2.f * UpperLen * ClampedTargetLen);
	A = FMath::Clamp(A, -1.f, 1.f);
	float ElbowAngle = FMath::Acos(A);

	FVector PoleDirWS = -UpperArm->GetRightVector();
	FVector PoleDirLS = Xform.InverseTransformVector(PoleDirWS).GetSafeNormal();

	FVector DirToTarget = RootToEff.GetSafeNormal();
	FVector PlaneNormal = FVector::CrossProduct(DirToTarget, PoleDirLS).GetSafeNormal();
	
	FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(ElbowAngle), PlaneNormal);
	
	FVector NewElbowPos = UpperArmPos + RotatedDir * UpperLen;
	FVector FixedHandPos  = NewElbowPos + (Target - NewElbowPos).GetSafeNormal() * LowerLen;
	
	Elbow->SetRelativeLocation(NewElbowPos);
	FVector HandWorldPos = Xform.TransformPosition(FixedHandPos);
	FVector HandLocalToElbow = Elbow->GetComponentTransform().InverseTransformPosition(HandWorldPos);
	Hand->SetRelativeLocation(HandLocalToElbow);
	
	FVector ElbowToHandLS  = (FixedHandPos  - NewElbowPos).GetSafeNormal();
	FVector PlaneNormalLS = FVector::CrossProduct(ElbowToHandLS, PoleDirLS).GetSafeNormal();
	FQuat ElbowRotation = FRotationMatrix::MakeFromXZ(ElbowToHandLS, PlaneNormalLS).ToQuat();

	//Elbow->SetRelativeRotation(ElbowRotation);
	Hand->SetRelativeRotation(TargetPose.Rotation);
}

bool UMoveArm::ShouldHoldUpHands()
{
	FTransform UpperArmWorld = this->GetComponentTransform();
	FVector StartWorld = UpperArmWorld.GetLocation();
	FVector EndWorld   = UpperArmWorld.GetLocation()  + GetRightVector() * 75;
	TargetPose.Position = FVector(0,25,  -50);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()->GetParentActor());
	Params.AddIgnoredActor(GetOwner());
	TArray<AActor*> ActorsAround; //  Gather actors
	UGameplayStatics::GetAllActorsWithTag (GetWorld(), "HandHeldItem", ActorsAround);
	for (AActor* Actor : ActorsAround)
	{
		Params.AddIgnoredActor(Actor);
	}
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		FVector LocalPoint = UpperArmWorld.InverseTransformPosition(Hit.ImpactPoint);
		TargetPose.Position = LocalPoint;

		FVector UpLocal = UpperArmWorld.InverseTransformVector(FVector::UpVector);
		FVector NormalLocal = UpperArmWorld.InverseTransformVector(Hit.ImpactNormal);

		//TargetPose.Rotation = FQuat::FindBetweenNormals(UpLocal, NormalLocal);
		return true;
	}
	FVector WorldPos = UpperArmWorld.TransformPosition(TargetPose.Position);
	//DrawDebugLine(GetWorld(), StartWorld, EndWorld, FColor::Red);
	//DrawDebugSphere(GetWorld(), WorldPos, 	8.f, 12, FColor::Red);
	return false;
}

