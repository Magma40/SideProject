// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/MovingModel.h"
#include "VectorTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/MainCharacter.h"
#include "ProcedualAnimation/MoveLeg.h"
#include "ProcedualAnimation/MoveArm.h"

AMovingModel::AMovingModel() { PrimaryActorTick.bCanEverTick = true; }

void AMovingModel::InitSceneComponents()
{
	// Get Hips
	Hips = FindChildByExactName(GetRootComponent(), TEXT("Hips"));
	if (!Hips)
	{
		UE_LOG(LogTemp, Error, TEXT("Hips not found"));
		return;
	}

	// Leg chains
	Legs = BuildLimbChains<UMoveLeg>(Hips,TEXT("UpperLeg_"),TEXT("Knee_"),TEXT("Foot_"));

	// Get Chest
	TArray<USceneComponent*> AllComponents;
	this->GetComponents<USceneComponent>(AllComponents);
	for (USceneComponent* Comp : AllComponents)
	{
		if (!Comp) continue;
		if (Comp->GetName() == TEXT("Chest")) Chest = Comp;
	}
	
	if (!Chest)
	{
		UE_LOG(LogTemp, Error, TEXT("Chest not found"));
		return;
	}
	// Arm chains
	Arms = BuildLimbChains<UMoveArm>( Chest,TEXT("UpperArm_"),TEXT("Elbow_"),TEXT("Hand_"));
}

void AMovingModel::BeginPlay()
{
	Super::BeginPlay();
	InitSceneComponents();
	const FTransform ParentXform = GetRootComponent()->GetComponentTransform();
	float HipLocalZ  = ParentXform.InverseTransformPosition(Hips->GetComponentLocation()).Z;
	float FootLocalZ = ParentXform.InverseTransformPosition(Legs[0].End->GetComponentLocation()).Z;
	BaseHipHeight = HipLocalZ - FootLocalZ;
	SetLegsIdleAnim();
	SetArmsIdleAnim();
}

void AMovingModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHips(DeltaTime);
	//DrawDebugJoints();
	//DrawDebugLimbs();
}

void AMovingModel::UpdateHips(float DeltaTime)
{
	if (Legs.Num() == 0 || !IsValid(Hips)) return;
	
	float HipSway =  UpdateLegs(DeltaTime);
	UpdateArms();
	
	//Apply New Hip Location
	FVector HipSwayOffset = GetActorForwardVector() * HipSway * HipSwayAmount;
	float DesiredHipHeight = BaseHipHeight;
	float HipHeight = DesiredHipHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f) * 0.5f;
	FVector NewHipLocalPos = FVector(0, 0, HipHeight) + HipSwayOffset;
	Hips->SetRelativeLocation(NewHipLocalPos);
}

float AMovingModel::UpdateLegs(float DeltaTime)
{
	float HipSway = 0.0f;
	//Leg Logic
	for (FLimbChain& Leg : Legs)
	{
		UMoveLeg* UpperLeg = Cast<UMoveLeg>(Leg.Upper);
		if (!IsValid(UpperLeg)) continue;
		if (UpperLeg->IsStepping())
		{
			float Lift = UpperLeg->GetLiftAmount();
			bool IsRight = UpperLeg->GetName().EndsWith(TEXT("_R"));
			HipSway = Lift * (IsRight ? 1.f : -1.f);
		}
	}
	return HipSway;
}

void AMovingModel::UpdateArms()
{
	//Arm Logic
	for (FLimbChain& Arm: Arms)
	{
		UMoveArm* UpperArm = Cast<UMoveArm>(Arm.Upper);
		if (!IsValid(UpperArm)) continue;
		UpperArm->UpdateArm();
	}
}

void AMovingModel::SetLegsIdleAnim()
{
	for (FLimbChain& Leg : Legs)
	{
		UMoveLeg* UpperLeg = Cast<UMoveLeg>(Leg.Upper);
		UpperLeg->SetIdleAnim();
	}
}
void AMovingModel::SetLegsMovingAnim()
{
	for (FLimbChain& Leg : Legs)
	{
		UMoveLeg* UpperLeg = Cast<UMoveLeg>(Leg.Upper);
		UpperLeg->SetMovingAnim();
	}
}
void AMovingModel::SetLegsJumpingAnim()
{
	for (FLimbChain& Leg : Legs)
	{
		UMoveLeg* UpperLeg = Cast<UMoveLeg>(Leg.Upper);
		ABaseInteractableCharacter* InteractableCharacter= Cast<ABaseInteractableCharacter>( GetParentActor());
		UpperLeg->SetJumpingAnim();
	}
}

void AMovingModel::SetArmsIdleAnim()
{
	for (FLimbChain& Arm : Arms)
	{
		UMoveArm* UpperArm = Cast<UMoveArm>(Arm.Upper);
		UpperArm->SetIdleAnim();
	}
}
void AMovingModel::SetArmsMovingAnim()
{
	for (FLimbChain& Arm : Arms)
	{
		UMoveArm* UpperArm = Cast<UMoveArm>(Arm.Upper);
		//UpperArm->Set();
	}
}
void AMovingModel::SetArmsWaveAnim()
{
	for (FLimbChain& Arm : Arms)
	{
		UMoveArm* UpperArm = Cast<UMoveArm>(Arm.Upper);
		if (UpperArm->GetName().EndsWith(TEXT("_R")))
		{
			UpperArm->SetWaveAnim();
		}
	}
}
void AMovingModel::SetArmsBackHolsterAnim()
{
	for (FLimbChain& Arm : Arms)
	{
		UMoveArm* UpperArm = Cast<UMoveArm>(Arm.Upper);
		ABaseInteractableCharacter* InteractableCharacter= Cast<ABaseInteractableCharacter>( GetParentActor());
		if (UpperArm->GetName().EndsWith(TEXT("_L")))
		{
			UpperArm->SetBackHolsterAnim(InteractableCharacter->BackHolster->GetComponentLocation());
		}
	}
}


void AMovingModel::DrawDebugJoints()
{
	FVector Pos_Hips = 					Hips->GetComponentLocation();
	FVector Pos_Chest =					Chest->GetComponentLocation();
	
	FVector Pos_UpperLeg_R =		FVector::ZeroVector;
	FVector Pos_Knee_R = 				FVector::ZeroVector;
	FVector Pos_Foot_R = 				FVector::ZeroVector;

	FVector Pos_UpperLeg_L =		FVector::ZeroVector;
	FVector Pos_Knee_L = 				FVector::ZeroVector;
	FVector Pos_Foot_L =				FVector::ZeroVector;

	FVector Pos_UpperArm_L =		FVector::ZeroVector;
	FVector Pos_Elbow_L = 			FVector::ZeroVector;
	FVector Pos_Hand_L =				FVector::ZeroVector;

	FVector Pos_UpperArm_R =		FVector::ZeroVector;
	FVector Pos_Elbow_R = 			FVector::ZeroVector;
	FVector Pos_Hand_R =				FVector::ZeroVector;

	for(FLimbChain& Leg : Legs)
	{
		if (Leg.Upper->GetName().EndsWith(TEXT("_R"))) Pos_UpperLeg_R = Leg.Upper->GetComponentLocation();
		else  Pos_UpperLeg_L = Leg.Upper->GetComponentLocation();

		if (Leg.Middle->GetName().EndsWith(TEXT("_R"))) Pos_Knee_R = Leg.Middle->GetComponentLocation();
		else  Pos_Knee_L = Leg.Middle->GetComponentLocation();

		if (Leg.End->GetName().EndsWith(TEXT("_R"))) Pos_Foot_R = Leg.End->GetComponentLocation();
		else  Pos_Foot_L = Leg.End->GetComponentLocation();
	}

	for(FLimbChain& Arm : Arms)
	{
		if (Arm.Upper->GetName().EndsWith(TEXT("_R"))) Pos_UpperArm_R = Arm.Upper->GetComponentLocation();
		else  Pos_UpperArm_L = Arm.Upper->GetComponentLocation();

		if (Arm.Middle->GetName().EndsWith(TEXT("_R"))) Pos_Elbow_R = Arm.Middle->GetComponentLocation();
		else  Pos_Elbow_L = Arm.Middle->GetComponentLocation();

		if (Arm.End->GetName().EndsWith(TEXT("_R"))) Pos_Hand_R = Arm.End->GetComponentLocation();
		else  Pos_Hand_L = Arm.End->GetComponentLocation();
	}

	DrawDebugSphere(GetWorld(), Pos_Hips, 				8.f, 12, FColor::Cyan);
	DrawDebugSphere(GetWorld(), Pos_Chest, 				8.f, 12, FColor::Cyan);

	DrawDebugSphere(GetWorld(), Pos_UpperLeg_R, 	8.f, 12, FColor::Green);
	DrawDebugSphere(GetWorld(), Pos_Knee_R, 			8.f, 12, FColor::Emerald);
	DrawDebugSphere(GetWorld(), Pos_Foot_R, 			8.f, 12, FColor::Purple);

	DrawDebugSphere(GetWorld(), Pos_UpperLeg_L, 	8.f, 12, FColor::Green);
	DrawDebugSphere(GetWorld(), Pos_Knee_L, 			8.f, 12, FColor::Emerald);
	DrawDebugSphere(GetWorld(), Pos_Foot_L, 			8.f, 12, FColor::Purple);

	DrawDebugSphere(GetWorld(), Pos_UpperArm_R, 	8.f, 12, FColor::Yellow);
	DrawDebugSphere(GetWorld(), Pos_Elbow_R, 			8.f, 12, FColor::Orange);
	DrawDebugSphere(GetWorld(), Pos_Hand_R, 			8.f, 12, FColor::Red);

	DrawDebugSphere(GetWorld(), Pos_UpperArm_L, 	8.f, 12, FColor::Yellow);
	DrawDebugSphere(GetWorld(), Pos_Elbow_L, 			8.f, 12, FColor::Orange);
	DrawDebugSphere(GetWorld(), Pos_Hand_L, 			8.f, 12, FColor::Red);
}
void AMovingModel::DrawDebugLimbs()
{
	FVector Pos_Hips = 					Hips->GetComponentLocation();
	FVector Pos_Chest =					Chest->GetComponentLocation();
	
	FVector Pos_UpperLeg_R =		FVector::ZeroVector;
	FVector Pos_Knee_R = 				FVector::ZeroVector;
	FVector Pos_Foot_R = 				FVector::ZeroVector;

	FVector Pos_UpperLeg_L =		FVector::ZeroVector;
	FVector Pos_Knee_L = 				FVector::ZeroVector;
	FVector Pos_Foot_L =				FVector::ZeroVector;

	FVector Pos_UpperArm_L =		FVector::ZeroVector;
	FVector Pos_Elbow_L = 			FVector::ZeroVector;
	FVector Pos_Hand_L =				FVector::ZeroVector;

	FVector Pos_UpperArm_R =		FVector::ZeroVector;
	FVector Pos_Elbow_R = 			FVector::ZeroVector;
	FVector Pos_Hand_R =				FVector::ZeroVector;

	for(FLimbChain& Leg : Legs)
	{
		if (Leg.Upper->GetName().EndsWith(TEXT("_R"))) Pos_UpperLeg_R = Leg.Upper->GetComponentLocation();
		else  Pos_UpperLeg_L = Leg.Upper->GetComponentLocation();

		if (Leg.Middle->GetName().EndsWith(TEXT("_R"))) Pos_Knee_R = Leg.Middle->GetComponentLocation();
		else  Pos_Knee_L = Leg.Middle->GetComponentLocation();

		if (Leg.End->GetName().EndsWith(TEXT("_R"))) Pos_Foot_R = Leg.End->GetComponentLocation();
		else  Pos_Foot_L = Leg.End->GetComponentLocation();
	}

	for(FLimbChain& Arm : Arms)
	{
		if (Arm.Upper->GetName().EndsWith(TEXT("_R"))) Pos_UpperArm_R = Arm.Upper->GetComponentLocation();
		else  Pos_UpperArm_L = Arm.Upper->GetComponentLocation();

		if (Arm.Middle->GetName().EndsWith(TEXT("_R"))) Pos_Elbow_R = Arm.Middle->GetComponentLocation();
		else  Pos_Elbow_L = Arm.Middle->GetComponentLocation();

		if (Arm.End->GetName().EndsWith(TEXT("_R"))) Pos_Hand_R = Arm.End->GetComponentLocation();
		else  Pos_Hand_L = Arm.End->GetComponentLocation();
	}

	DrawDebugLine(GetWorld(), Pos_Hips, Pos_Chest, FColor::Cyan);
	
	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_R,	 FColor::Green);
	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_L, FColor::Green);

	DrawDebugLine(GetWorld(), Pos_Chest, Pos_UpperArm_R,	 FColor::Yellow);
	DrawDebugLine(GetWorld(), Pos_Chest, Pos_UpperArm_L, FColor::Yellow);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_R, Pos_Knee_R, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_R, Pos_Foot_R, FColor::Purple);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_L, Pos_Knee_L, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_L, Pos_Foot_L, FColor::Purple);

	DrawDebugLine(GetWorld(), Pos_UpperArm_R, Pos_Elbow_R, FColor::Orange);
	DrawDebugLine(GetWorld(), Pos_Elbow_R, Pos_Hand_R,  FColor::Red);

	DrawDebugLine(GetWorld(), Pos_UpperArm_L, Pos_Elbow_L, FColor::Orange);
	DrawDebugLine(GetWorld(), Pos_Elbow_L, Pos_Hand_L, FColor::Red);
}

USceneComponent* AMovingModel::FindChildByExactName(const USceneComponent* Parent, const FString& Target)
{
	TArray<USceneComponent*> FindChildren;
	Parent->GetChildrenComponents(false, FindChildren);

	for (USceneComponent* Child : FindChildren)
		if (Child->GetName() == Target)return Child;

	return nullptr;
}

 TArray<USceneComponent*> AMovingModel::FindChildrenByPrefix(const USceneComponent* Parent, const FString& Prefix)
{
	TArray<USceneComponent*> Matches;
	TArray<USceneComponent*> FindChildren;
	Parent->GetChildrenComponents(false, FindChildren);

	for (USceneComponent* Child : FindChildren)
		if (Child->GetName().StartsWith(Prefix))
			Matches.Add(Child);

	return Matches;
}

template<typename TUpperClass>
TArray<FLimbChain> AMovingModel::BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix) const
{
	TArray<FLimbChain> OutChains;

	TArray<USceneComponent*> UpperList = FindChildrenByPrefix(Root, UpperPrefix);

	for (USceneComponent* UpperComp : UpperList)
	{
		TUpperClass* Upper = Cast<TUpperClass>(UpperComp);
		if (!Upper) continue;

		FLimbChain Chain;
		Chain.Upper = Upper;

		TArray<USceneComponent*> Middles = FindChildrenByPrefix(UpperComp, MiddlePrefix);
		if (Middles.Num() == 0) continue;
		Chain.Middle = Middles[0];

		TArray<USceneComponent*> Ends = FindChildrenByPrefix(Chain.Middle, EndPrefix);
		if (Ends.Num() == 0) continue;
		Chain.End = Ends[0];

		OutChains.Add(Chain);
	}

	return OutChains;
}