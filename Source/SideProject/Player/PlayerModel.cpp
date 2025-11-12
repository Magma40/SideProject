// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerModel.h"
#include "LandscapeDataAccess.h"
#include "MainCharacter.h"
#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProcedualAnimation/Leg.h"

// Sets default values
APlayerModel::APlayerModel()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

// Called when the game starts or when spawned
void APlayerModel::BeginPlay()
{
	Super::BeginPlay();
	GetComponents(Legs);
	Hip = FindChild<USceneComponent>(GetRootComponent(),		FName("Hips"));
	USceneComponent* Foot = FindChild<USceneComponent>(GetRootComponent(),		FName("Foot_R"));
	if (Hip)
	{
		HipBaseHeight = Hip->GetComponentLocation().Z - Foot->GetComponentLocation().Z;
	}
	MoveTarget  = UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass());
}

// Called every frame
void APlayerModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Legs.Num() == 0) return;

	// --- Raycast to find MoveTarget ground position ---
	FVector TargetPos = MoveTarget->GetActorLocation();
	FVector Start = TargetPos + FVector::UpVector * 100.f;
	FVector End = TargetPos + FVector::DownVector * 200.f;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		TargetPos = Hit.ImpactPoint + Hit.ImpactNormal * 2.f; 
	}

	// --- Update movement ---
	FVector CurrentPos = GetActorLocation();
	FVector NewPos = FMath::VInterpConstantTo(CurrentPos, TargetPos, DeltaTime, MoveSpeed * 100.f); // convert to uu/s
	SetActorLocation(NewPos);

	// --- Update rotation ---
	FQuat CurrentRot = GetActorQuat();
	FQuat TargetRot =  MoveTarget->GetActorQuat();
	if (UE::Geometry::Distance(MoveTarget->GetActorLocation(), GetActorLocation())  > 50.f)
	{
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MoveTarget->GetActorLocation());
		FQuat NewRot = FQuat::Slerp(CurrentRot, TargetRot, DeltaTime * RotationSpeed);
		PlayerRot.Pitch = 0.0;
		SetActorRotation(PlayerRot);	
	}

	// --- Calculate feet center & lowest foot ---
	FVector FeetCenter = FVector::ZeroVector;
	float LowestFoot = TNumericLimits<float>::Max();
	float HipSway = 0.f;

	for (ULeg* Leg : Legs)
	{
		if (!Leg) continue;
	
		Leg->UpdateLeg();
	
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

	float DesiredHipHeight = LowestFoot + HipBaseHeight ;
	float HipHeight = DesiredHipHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.f) * 2.f;
	FVector HipWorldPos = FVector(FeetCenter.X, FeetCenter.Y, HipHeight + HipsApplyHeight) + HipSwayOffset;
//Hip->SetWorldLocation(HipWorldPos);
	DrawDebugSphere(GetWorld(), HipWorldPos, 8.f, 12, FColor::Cyan);

	// for (ULeg* Leg : Legs)
	// {
	// 	if (Leg)
	// 		Leg->UpdateLeg();
	// }
}


