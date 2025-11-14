// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MovingModel.h"

#include "Leg.h"
#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/MainCharacter.h"
#include "ProcedualAnimation/MoveLeg.h"

AMovingModel::AMovingModel() { PrimaryActorTick.bCanEverTick = true; }

void AMovingModel::Init() // Make this is disgusting
{
	Legs .Empty();
	Knees.Empty();
	Feet.Empty();
	
	TArray<USceneComponent*> RootChildren; // Get Hips
	GetRootComponent()->GetChildrenComponents(true, RootChildren);
	for (USceneComponent* Child : RootChildren)
	{
		if (Child->GetName() == TEXT("Hips")) Hips = Child; break;
	}
	if (!IsValid(Hips))
	{
		UE_LOG(LogTemp, Error, TEXT("Hips component not found!"));
		return;
	}

	TArray<USceneComponent*> HipChildren; //Get UpperLegs
	Hips->GetChildrenComponents(false, HipChildren);
	for (USceneComponent* Child : HipChildren)
	{
		if (Child->GetName().Contains(TEXT("UpperLeg_")))
		{
			UMoveLeg* Leg = Cast<UMoveLeg>(Child);
			if (IsValid(Leg)) Legs.Add(Leg); 
			else  UE_LOG(LogTemp, Error, TEXT("UpperLeg component is NOT a UMoveLeg: %s"), *Child->GetName());
		}
	}

	for (UMoveLeg* Leg : Legs) // Get Knees
	{
		TArray<USceneComponent*> LegChildren;
		Leg->GetChildrenComponents(false, LegChildren);
		for (USceneComponent* Child : LegChildren)
		{
			if (Child->GetName().Contains(TEXT("Knee_"))) Knees.Add(Child);
		}	
	}

	for (USceneComponent* Knee : Knees) // Get Feet
	{
		TArray<USceneComponent*> KneeChildren;
		Knee->GetChildrenComponents(false, KneeChildren);
		for (USceneComponent* Child : KneeChildren)
		{
			if (Child->GetName().Contains(TEXT("Foot_"))) Feet.Add(Child); 
		}	
	}
}

void AMovingModel::BeginPlay()
{
	Super::BeginPlay();
	Init();

	BaseHipHeight = (Hips->GetRelativeLocation().Z - Feet[0]->GetRelativeLocation().Z);
	MoveTarget  = UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass());
}

void AMovingModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHips();
	UpdateMovement(DeltaTime);
	
	DrawDebugJoints();
	DrawDebugLimbs();
}

void AMovingModel::UpdateHips()
{
	if (Legs.Num() == 0 || !IsValid(Hips)) return;
	
	//NOTE : This is in local space
	FVector FeetCenter = FVector::ZeroVector;
	float LowestFoot = TNumericLimits<float>::Max();
	float HipSway = 0.0f;
	USceneComponent* Root = GetRootComponent();
	const FTransform RootWorld = Root->GetComponentTransform();

	//Leg Logic
	for (UMoveLeg* Leg : Legs)
	{
		if (!IsValid(Leg)) continue;
		Leg->UpdateLeg();
		FVector FootWorldPos  = Leg->GetTargetPose().Position;
		FVector FootLocalPos = Root->GetComponentTransform().InverseTransformPosition(FootWorldPos);
		//FVector LocalFootPos = Leg->GetComponentTransform().TransformPosition(Leg->GetTargetPose().Position);
		FeetCenter += FootLocalPos;
		 LowestFoot = FMath::Min(LowestFoot, FootLocalPos.Z);
		if (Leg->IsStepping()) HipSway = Leg->GetLiftAmount() * (Leg->GetName().EndsWith(TEXT("_R")) ? 1.f : -1.f);
	}
	FeetCenter /= Legs.Num();

	float DesiredHipHeight = LowestFoot + BaseHipHeight;
	float CurrentHipZ = Hips->GetRelativeLocation().Z;
	float HipHeight = FMath::FInterpTo(CurrentHipZ, DesiredHipHeight, GetWorld()->GetDeltaSeconds(), 5.f);
	
	//Apply New Hip Location
	FVector Right = GetActorRightVector();
	FVector HipSwayLocal(0, HipSway * HipSwayAmount, 0);
	FVector HipSwayOffset = FVector(0, HipSway * HipSwayAmount, 0); // Local Y axis

	//float HipHeight = DesiredHipHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f) * 2.0f;
	FVector NewHipLocalPos = FVector(FeetCenter.X, FeetCenter.Y, HipHeight) + HipSwayOffset;;
	//FVector LocalHipPos = GetRootComponent()->GetComponentTransform().InverseTransformPosition(NewHipLocalPos);
	Hips->SetRelativeLocation(NewHipLocalPos);
}

void AMovingModel::UpdateMovement(float DeltaTime)
{
	if (!IsValid(MoveTarget)) return;
	if (UE::Geometry::Distance(MoveTarget->GetActorLocation(), GetActorLocation())  < 200.f) return;
	
	//Raycast to find MoveTarget ground position
	FVector TargetPos = MoveTarget->GetActorLocation();
	FVector Start = TargetPos + FVector::UpVector * 100.f;
	FVector End = TargetPos + FVector::DownVector * 200.f;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		TargetPos = Hit.ImpactPoint + Hit.ImpactNormal * 2.f; 
	}

	//Update Movement
	FVector CurrentPos = GetActorLocation();
	FVector NewPos = FMath::VInterpConstantTo(CurrentPos, TargetPos, DeltaTime, MoveSpeed * 100.f);
	SetActorLocation(NewPos);

	//Update Rotation
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MoveTarget->GetActorLocation());
	LookAtRot.Pitch = 0.0;
	LookAtRot.Yaw -= 90.f;
	SetActorRotation(LookAtRot);
}

void AMovingModel::DrawDebugJoints()
{
	FVector Pos_Hips = 				Hips->GetComponentLocation();
	FVector Pos_UpperLeg_R = FVector::ZeroVector;
	FVector Pos_Knee_R = 			FVector::ZeroVector;
	FVector Pos_Foot_R = 			FVector::ZeroVector;

	FVector Pos_UpperLeg_L =	FVector::ZeroVector;
	FVector Pos_Knee_L = 			FVector::ZeroVector;
	FVector Pos_Foot_L =			FVector::ZeroVector;

	for(UMoveLeg* Leg : Legs)
	{
		if (Leg->GetName().EndsWith(TEXT("_R"))) Pos_UpperLeg_R = Leg->GetComponentLocation();
		else  Pos_UpperLeg_L = Leg->GetComponentLocation();
	}
	for(USceneComponent* Knee : Knees)
	{
		if (Knee->GetName().EndsWith(TEXT("_R"))) Pos_Knee_R = Knee->GetComponentLocation();
		else  Pos_Knee_L = Knee->GetComponentLocation();
	}
	for(USceneComponent* Foot : Feet)
	{
		if (Foot->GetName().EndsWith(TEXT("_R"))) Pos_Foot_R = Foot->GetComponentLocation();
		else  Pos_Foot_L = Foot->GetComponentLocation();
	}

	DrawDebugSphere(GetWorld(), Pos_Hips, 				8.f, 12, FColor::Cyan);

	DrawDebugSphere(GetWorld(), Pos_UpperLeg_R, 	8.f, 12, FColor::Green);
	DrawDebugSphere(GetWorld(), Pos_Knee_R, 			8.f, 12, FColor::Emerald);
	DrawDebugSphere(GetWorld(), Pos_Foot_R, 			8.f, 12, FColor::Purple);

	DrawDebugSphere(GetWorld(), Pos_UpperLeg_L, 	8.f, 12, FColor::Green);
	DrawDebugSphere(GetWorld(), Pos_Knee_L, 			8.f, 12, FColor::Emerald);
	DrawDebugSphere(GetWorld(), Pos_Foot_L, 			8.f, 12, FColor::Purple);
}
void AMovingModel::DrawDebugLimbs()
{
	FVector Pos_Hips = 				Hips->GetComponentLocation();
	FVector Pos_UpperLeg_R = FVector::ZeroVector;
	FVector Pos_Knee_R = 			FVector::ZeroVector;
	FVector Pos_Foot_R = 			FVector::ZeroVector;

	FVector Pos_UpperLeg_L =	FVector::ZeroVector;
	FVector Pos_Knee_L = 			FVector::ZeroVector;
	FVector Pos_Foot_L =			FVector::ZeroVector;

	for(UMoveLeg* Leg : Legs)
	{
		if (Leg->GetName().EndsWith(TEXT("_R"))) Pos_UpperLeg_R = Leg->GetComponentLocation();
		else  Pos_UpperLeg_L = Leg->GetComponentLocation();
	}
	for(USceneComponent* Knee : Knees)
	{
		if (Knee->GetName().EndsWith(TEXT("_R"))) Pos_Knee_R = Knee->GetComponentLocation();
		else  Pos_Knee_L = Knee->GetComponentLocation();
	}
	for(USceneComponent* Foot : Feet)
	{
		if (Foot->GetName().EndsWith(TEXT("_R"))) Pos_Foot_R = Foot->GetComponentLocation();
		else  Pos_Foot_L = Foot->GetComponentLocation();
	}

	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_R, FColor::Cyan);
	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_L, FColor::Cyan);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_R, Pos_Knee_R, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_L, Pos_Foot_L, FColor::Purple);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_L, Pos_Knee_L, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_R, Pos_Foot_R, FColor::Purple);
}

