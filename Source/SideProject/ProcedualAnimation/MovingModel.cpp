// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MovingModel.h"
#include "ProcedualAnimation/MoveLeg.h"

AMovingModel::AMovingModel() { PrimaryActorTick.bCanEverTick = true; }

void AMovingModel::Init()
{
	Legs .Empty();
	TArray<USceneComponent*> FindChildren;
	GetRootComponent()->GetChildrenComponents(true, FindChildren);
	for (USceneComponent* Child : FindChildren)
	{
		if (Child->GetName() == TEXT("Hips")) Hips = Child;

		if (Child->GetName() == TEXT("UpperLeg_R")) 	UpperLeg_R = Child; 	Legs.Add(Cast<UMoveLeg>(Child));
		if (Child->GetName() == TEXT("Knee_R")) 			Knee_R = Child; 				
		if (Child->GetName() == TEXT("Foot_R"))				Foot_R = Child; 					

		if (Child->GetName() == TEXT("UpperLeg_L"))		UpperLeg_L = Child; 		Legs.Add(Cast<UMoveLeg>(Child));
		if (Child->GetName() == TEXT("Knee_L")) 			Knee_L = Child; 				
		if (Child->GetName() == TEXT("Foot_L"))				Foot_L = Child; 					
	}	

	Pos_Hips = 				Hips->GetComponentLocation();

	Pos_UpperLeg_R = 	UpperLeg_R->GetComponentLocation();
	Pos_Knee_R = 			Knee_R->GetComponentLocation();
	Pos_Foot_R = 			Foot_R->GetComponentLocation();

	Pos_UpperLeg_L =	UpperLeg_L->GetComponentLocation();
	Pos_Knee_L = 			Knee_L->GetComponentLocation();
 	Pos_Foot_L =				Foot_L->GetComponentLocation();
}

void AMovingModel::BeginPlay()
{
	Super::BeginPlay();
	Init();

	BaseHipHeight = Hips->GetComponentLocation().Z - Foot_R->GetComponentLocation().Z;
}

void AMovingModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHips(DeltaTime);
	
	DrawDebugJoints();
	DrawDebugLimbs();
}

void AMovingModel::UpdateHips(float DeltaTime)
{
	//NOTE : This is in local space
	FVector FeetCenter = FVector::ZeroVector;
	float LowestFoot = TNumericLimits<float>::Max();
	float HipSway = 0.0f;

	//Leg Logic
	for (UMoveLeg* Leg : Legs)
	{
		if (!IsValid(Leg)) continue;
		Leg->UpdateLeg();
		FVector LocalFootPos = GetTransform().InverseTransformPosition(Leg->GetTargetPose().Position);
		FeetCenter += LocalFootPos;
		LowestFoot = FMath::Min(LowestFoot, LocalFootPos.Z);
		//if (Leg->IsStepping()) HipSway = Leg->GetLiftAmount() * (Leg->GetName().EndsWith(TEXT("_R")) ? 1.f : -1.f);
	}
	FeetCenter /= Legs.Num();

	//Apply New Hip Location
	FVector Right = GetActorRightVector();
	FVector HipSwayOffset = Right * HipSway * HipSwayAmount;

	float DesiredHipHeight = LowestFoot + BaseHipHeight;
	float HipHeight = DesiredHipHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f) * 2.0f;
	FVector NewHipLocalPos = FVector(FeetCenter.X, FeetCenter.Y, HipHeight) + HipSwayOffset;
	Hips->SetRelativeLocation(NewHipLocalPos);
}

void AMovingModel::DrawDebugJoints()
{
	Pos_Hips = 				Hips->GetComponentLocation();

	Pos_UpperLeg_R = 	UpperLeg_R->GetComponentLocation();
	Pos_Knee_R = 			Knee_R->GetComponentLocation();
	Pos_Foot_R = 			Foot_R->GetComponentLocation();

	Pos_UpperLeg_L =	UpperLeg_L->GetComponentLocation();
	Pos_Knee_L = 			Knee_L->GetComponentLocation();
	Pos_Foot_L =				Foot_L->GetComponentLocation();

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
	Pos_Hips = 				Hips->GetComponentLocation();

	Pos_UpperLeg_R = 	UpperLeg_R->GetComponentLocation();
	Pos_Knee_R = 			Knee_R->GetComponentLocation();
	Pos_Foot_R = 			Foot_R->GetComponentLocation();

	Pos_UpperLeg_L =	UpperLeg_L->GetComponentLocation();
	Pos_Knee_L = 			Knee_L->GetComponentLocation();
	Pos_Foot_L =				Foot_L->GetComponentLocation();

	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_R, FColor::Cyan);
	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_L, FColor::Cyan);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_R, Pos_Knee_R, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_L, Pos_Foot_L, FColor::Purple);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_L, Pos_Knee_L, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_R, Pos_Foot_R, FColor::Purple);
}

