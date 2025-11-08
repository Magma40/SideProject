// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerModel.h"
#include "LandscapeDataAccess.h"
#include "ProcedualAnimation/Leg.h"

// Sets default values
APlayerModel::APlayerModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void APlayerModel::BeginPlay()
{
	Super::BeginPlay();
	HipsInModel = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("Leg_L"));
	Waist = FindChild<UStaticMeshComponent>(GetRootComponent(),			FName("Waist"));
	Neck = FindChild<UStaticMeshComponent>(GetRootComponent(),			FName("Neck"));

	Leg_L = FindChild<UStaticMeshComponent>(GetRootComponent(),			FName("Leg_L"));
	LegJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("LegJoint_L"));
	FootJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("FootJoint_L"));
	
	Leg_R = FindChild<UStaticMeshComponent>(GetRootComponent(),			FName("Leg_R"));
	LegJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("LegJoint_R"));
	FootJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("FootJoint_R"));
	
	Arm_L = FindChild<UStaticMeshComponent>(GetRootComponent(),			FName("Arm_L"));
	ArmJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("ArmJoint_L"));
	HandJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("HandJoint_L"));

	Arm_R = FindChild<UStaticMeshComponent>(GetRootComponent(),			FName("Arm_R"));
	ArmJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("ArmJoint_R"));
	HandJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(),		FName("HandJoint_R"));

	MoveTarget = FTransform(GetActorRotation(), GetActorLocation());
	HipSwayAmount = HipsInModel->GetRelativeLocation().Z;
	Legs.Push(FindChild<ULeg>(GetRootComponent(),					FName("Leg_R")));
	Legs.Push(FindChild<ULeg>(GetRootComponent(),					FName("Leg_L")));
}

// Called every frame
void APlayerModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Legs.IsEmpty()) return;

	FVector MoveTargetRef = MoveTarget.GetLocation();
	FVector Start = MoveTargetRef + FVector::UpVector * 100.0f;
	FVector End = Start + FVector::DownVector * 200.0f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(2.0f),
		Params
	);
	if (bHit){ MoveTargetRef = HitResult.ImpactPoint + FVector::UpVector * 2.0f; }

	// Move toward target
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = MoveTargetRef;
	float MoveStep = MoveSpeed * DeltaTime;

	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

	SetActorLocation(NewLocation);

	//Rotate toward target
	FQuat CurrentRotation = GetActorQuat();
	FQuat TargetRotation = MoveTarget.GetRotation(); // assuming MoveTarget is FTransform

	FQuat NewRotation = FMath::QInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation);

	FVector FeetCenter = FVector::ZeroVector;
	float HipSway = 0.0f;
	float LowestFoot = TNumericLimits<float>::Max();

	if (Legs.Num() > 0)
	{
		for (ULeg* Leg : Legs)
		{
			if (!IsValid(Leg)) continue;
			Leg->UpdateLeg(DeltaTime);

			FeetCenter += Leg->GetGroundPose().Position;
			LowestFoot = FMath::Min(LowestFoot, Leg->GetGroundPose().Position.Z);

			if (Leg->bIsStepping)
			{
				HipSway = Leg->LiftAmount * (Leg->GetOwner()->GetActorLabel().EndsWith(TEXT("_R")) ? 1.0f : -1.0f);
			}
		}

		 FeetCenter /= Legs.Num();
	}

	FVector Right = GetActorRightVector();
	FVector HipSwayRef = Right * HipSway * HipSwayAmount;

	float TimeSeconds = GetWorld()->GetTimeSeconds();
	float Oscillation = FMath::Sin(TimeSeconds * 2.0f) * 0.2f;
	
	float HipHeightRef = LowestFoot + HipHeight + Oscillation;

	FVector NewHipLocation(FeetCenter.X, FeetCenter.Y, HipHeightRef);
	NewHipLocation += HipSwayRef;

	if (IsValid(HipsInModel))
	{
		HipsInModel->SetWorldLocation(NewHipLocation);
	}

	for (ULeg* Leg : Legs)
	{
		if (Leg)
		{
			Leg->UpdateLeg(DeltaTime);
		}
	}
}


