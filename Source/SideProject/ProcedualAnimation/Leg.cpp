// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/Leg.h"
#include "DrawDebugHelpers.h"
#include "SkeletalMeshAttributes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerModel.h"

ULeg::ULeg()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULeg::BeginPlay()
{
	Super::BeginPlay();
	APlayerModel* PlayerModel = Cast<APlayerModel>(GetAttachmentRootActor());
	//APlayerModel PlayerModel;
	//Bones.Init(nullptr, 3);
	if (GetName() == FName("Leg_R"))
	{
		Bones[0] = GetAttachParent();
		Bones[1] = PlayerModel->FindChild<USceneComponent>(this, FName("LegJoint_R"));
		Bones[2] = PlayerModel->FindChild<USceneComponent>(this, FName("FootJoint_R"));
	}
	else if (GetName() == FName("Leg_L"))
	{
		Bones[0] = GetAttachParent();
		Bones[1] = PlayerModel->FindChild<USceneComponent>(this, FName("LegJoint_L"));
		Bones[2] = PlayerModel->FindChild<USceneComponent>(this, FName("FootJoint_L"));
	}
	
	if (Bones.Num() == 3 && Bones[0] && Bones[1] && Bones[2])
	{
		LocalGroundPosition = FVector(0, 0, -GroundOffset);
		TargetPose = GetGroundPose();
	}
}

void ULeg::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsStepping && ShouldStep())
	{
		DoStep(TargetPose, GetGroundPose());
	}
	
	UpdateLeg(DeltaTime);
}

FLegPose ULeg::GetGroundPose() const
{
	FVector Start = GetOwner()->GetActorTransform().TransformPosition(LocalGroundPosition + FVector(0, 0, 50.f));
	FVector End = Start + FVector(0, 0, -100.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FLegPose Pose;
	Pose.Position = End;
	Pose.Rotation = FQuat::Identity;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		Pose.Position = Hit.ImpactPoint + Hit.ImpactNormal * GroundOffset;
		Pose.Rotation = FQuat::FindBetweenNormals(FVector::UpVector, Hit.ImpactNormal);
	}

	return Pose;
}

bool ULeg::ShouldStep() const
{
	float Dist = FVector::Dist(TargetPose.Position, GetGroundPose().Position);
	return Dist > StepTrigger;
}

void ULeg::UpdateLeg(float DeltaTime)
{
	if (Bones.Num() != 3) return;

	// Example simplified IK: just point foot toward target
	FVector Start = Bones[0]->GetComponentLocation();
	FVector End = TargetPose.Position;
	Bones[2]->SetWorldLocation(End);
	Bones[2]->SetWorldRotation(TargetPose.Rotation.Rotator());

	DrawDebugSphere(GetWorld(), TargetPose.Position, 5.f, 8, FColor::Yellow);
}

void ULeg::DoStep(const FLegPose& Source, const FLegPose& Target)
{
	bIsStepping = true;

	float Elapsed = 0.f;
	float Duration = StepTime;

	GetWorld()->GetTimerManager().SetTimer(
		StepTimerHandle,
		[this, Source, Target, Duration, Elapsed]() mutable
		{
			Elapsed += GetWorld()->GetDeltaSeconds();
			float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);

			// Parabolic lift curve
			float Lift = FMath::Sin(Alpha * PI) * StepHeight;
			FVector Pos = FMath::Lerp(Source.Position, Target.Position, Alpha) + FVector(0, 0, Lift);
			FQuat Rot = FQuat::Slerp(Source.Rotation, Target.Rotation, Alpha);

			TargetPose.Position = Pos;
			TargetPose.Rotation = Rot;

			if (Alpha >= 1.f)
			{
				bIsStepping = false;
				GetWorld()->GetTimerManager().ClearTimer(StepTimerHandle);
			}
		},
		0.01f, true);
}
