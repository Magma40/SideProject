// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerModel.h"
#include "LandscapeDataAccess.h"
#include "ProcedualAnimation/Leg.h"

// Sets default values
APlayerModel::APlayerModel()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	MoveTarget = GetActorTransform();
}

// Called when the game starts or when spawned
void APlayerModel::BeginPlay()
{
	Super::BeginPlay();
	GetComponents(Legs);
	Hip = FindChild<USceneComponent>(GetRootComponent(),		FName("Hips"));
	if (Hip)
	{
		HipBaseHeight = Hip->GetComponentLocation().Z;
	}
}

// Called every frame
void APlayerModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Legs.Num() == 0) return;

	// --- Raycast to find MoveTarget ground position ---
	FVector TargetPos = MoveTarget.GetLocation();
	FVector Start = TargetPos + FVector::UpVector * 100.f;
	FVector End = TargetPos + FVector::DownVector * 200.f;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		TargetPos = Hit.ImpactPoint + Hit.ImpactNormal * 2.f; // Same as Unity spherecast offset
	}

	// --- Update movement ---
	FVector CurrentPos = GetActorLocation();
	FVector NewPos = FMath::VInterpConstantTo(CurrentPos, TargetPos, DeltaTime, MoveSpeed * 100.f); // convert to uu/s
	//SetActorLocation(NewPos);

	// --- Update rotation ---
	FQuat CurrentRot = GetActorQuat();
	FQuat TargetRot = MoveTarget.GetRotation();
	FQuat NewRot = FQuat::Slerp(CurrentRot, TargetRot, DeltaTime * RotationSpeed);
	//SetActorRotation(NewRot);

	// --- Calculate feet center & lowest foot ---
	FVector FeetCenter = FVector::ZeroVector;
	float LowestFoot = TNumericLimits<float>::Max();
	float HipSway = 0.f;

	for (ULeg* Leg : Legs)
	{
		if (!Leg) continue;

		Leg->UpdateLeg(); // Equivalent to leg.UpdateLeg();

		FeetCenter += Leg->GetTargetPose().Position;
		LowestFoot = FMath::Min(LowestFoot, Leg->FootBottom().Z);

		if (Leg->IsStepping())
		{
			HipSway = Leg->GetLiftAmount() * (Leg->GetName().EndsWith(TEXT("_R")) ? 1.f : -1.f);
		}
	}
	FeetCenter /= Legs.Num();

	// --- Hip sway and vertical motion ---
	FVector Right = GetActorForwardVector();
	FVector HipSwayOffset = Right * HipSway * HipSwayAmount;

	float DesiredHipHeight = LowestFoot + HipBaseHeight;
	float CurrentHipZ = Hip->GetComponentLocation().Z;
	//float HipHeight = FMath::FInterpTo(CurrentHipZ, DesiredHipHeight, DeltaTime, HipSmoothSpeed);
	//float HipHeight = LowestFoot + HipBaseHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.f) * 20.f;
	FVector HipWorldPos = FVector(FeetCenter.X, FeetCenter.Y, CurrentHipZ); //+ HipSwayOffset;

	Hip->SetWorldLocation(HipWorldPos);

	// --- Optional: visualize hip position ---
	DrawDebugSphere(GetWorld(), HipWorldPos, 8.f, 12, FColor::Cyan);

	// --- Update legs again for final foot alignment ---
	for (ULeg* Leg : Legs)
	{
		if (Leg)
			Leg->UpdateLeg();
	}
}


