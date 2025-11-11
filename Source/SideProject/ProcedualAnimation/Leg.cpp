// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/Leg.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerModel.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ULeg::ULeg()
{
	PrimaryComponentTick.bCanEverTick = true;

	StepTrigger = 2.0f;
	StepTime = 0.5f;
	StepHeight = 2.0f;
	GroundOffset = 0.5f;
	bIsStepping = false;
	LiftAmount = 0.f;
}

void ULeg::BeginPlay()
{
	Super::BeginPlay();
	AActor* FoundActor  = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerModel::StaticClass());
	APlayerModel* PlayerModel = Cast<APlayerModel>(FoundActor);
	if (!PlayerModel) return;

	Bones.Empty();
	Bones.Add(this);
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);
	if (GetName().Contains(TEXT("Leg_L")))
	{
		for (USceneComponent* Child : Children)
		{
			if (Child->GetName() == TEXT("LegJoint_L")) Bones.Add(Child);
			if (Child->GetName() == TEXT("FootJoint_L")) Bones.Add(Child);
		}	
	}
	else
	{
		if (GetName().Contains(TEXT("Leg_R")))
		{
			for (USceneComponent* Child : Children)
			{
				if (Child->GetName() == TEXT("LegJoint_R")) Bones.Add(Child);
				if (Child->GetName() == TEXT("FootJoint_R")) Bones.Add(Child);
			}	
		}
	}
}

void ULeg::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsStepping && ShouldStep()) StartStep();
	if (bIsStepping)  UpdateStep(DeltaTime);
	UpdateLeg();
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
	if (Dist < StepTrigger) return false;

	int32 SteppingLegs = 0;
	for (ULeg* Leg : OtherLegs)
	{
		if (Leg && Leg->IsStepping()) SteppingLegs++;
	}

	if (SteppingLegs > 0) return false;
	return true;
}

void ULeg::StartStep()
{
	bIsStepping = true;
	StepElapsed = 0.f;
	SourcePose = TargetPose;
	TargetPose = GetGroundPose();

	// Add a per-leg offset so legs don't land on the exact same spot
	FVector LegOffset = FVector::ZeroVector;
	if (GetName().Contains(TEXT("Leg_L"))) LegOffset = FVector(0.0f, -10.f, 0.f);   // left leg offset
	else if (GetName().Contains(TEXT("Leg_R"))) LegOffset = FVector(0.0f, 10.f, 0.f); // right leg offset

	TargetPose.Position += LegOffset;
}

void ULeg::UpdateStep(float DeltaTime)
{
	StepElapsed += DeltaTime;
	float Alpha = FMath::Clamp(StepElapsed / StepTime, 0.f, 1.f);

	LiftAmount = FMath::Sin(Alpha * PI) * StepHeight;

	FVector Pos = FMath::Lerp(SourcePose.Position, TargetPose.Position, Alpha) + FVector::UpVector * LiftAmount;
	FQuat Rot = FQuat::Slerp(SourcePose.Rotation, TargetPose.Rotation, Alpha);

	TargetPose.Position = Pos;
	TargetPose.Rotation = Rot;

	if (Alpha >= 1.f)
	{
		bIsStepping = false;
		LiftAmount = 0.f;
	}
}

void ULeg::UpdateLeg()
{
	if (Bones.Num() != 3) return;
	if (!IsValid(Bones[0]) || !IsValid(Bones[1]) || !IsValid(Bones[2])) return;
	USceneComponent* Hip = Bones[0];
	USceneComponent* Knee = Bones[1];
	USceneComponent* Foot = Bones[2];

	FVector HipPos = Hip->GetComponentLocation();
	//HipPos -= Hip->GetUpVector() * 20;
	//HipPos.Z -= 20;
	FVector KneePos = Knee->GetComponentLocation();
	//KneePos.Z -= 20;
	FVector FootPos = Foot->GetComponentLocation();
	FVector Target = TargetPose.Position;
	
	// Calculate pole direction (approx same as Unity)
		FVector Forward = Hip->GetForwardVector();
		FVector ToUpperLeg = (KneePos - HipPos).GetSafeNormal();
		FVector Pole = HipPos + -Hip->GetRightVector() * 50.f;// - FVector(0, 0, 200.f);
	
	 // --- TWO BONE IK ---
		FVector RootToEff = Target - HipPos;
		float UpperLen = (KneePos - HipPos).Size();
		float LowerLen = (FootPos - KneePos).Size();
		float TargetLen = RootToEff.Size();
		TargetLen = FMath::Clamp(TargetLen, FMath::Abs(UpperLen - LowerLen) + 1e-3f, UpperLen + LowerLen - 1e-3f);
	
	// Law of cosines for knee angle
		float A = (UpperLen * UpperLen + TargetLen * TargetLen - LowerLen * LowerLen) / (2.0f * UpperLen * TargetLen);
		A = FMath::Clamp(A, -1.f, 1.f);
		float KneeAngle = FMath::Acos(A);

		FVector ForwardPoleDir = (Hip->GetRightVector() - Hip->GetUpVector()).GetSafeNormal();
		FVector DirToTarget = (Target - HipPos).GetSafeNormal();
		FVector PlaneNormal = FVector::CrossProduct(DirToTarget, ForwardPoleDir).GetSafeNormal();
	
		if (PlaneNormal.IsNearlyZero()) PlaneNormal = FVector::UpVector;
	
		FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle + 90), PlaneNormal);
		FVector NewKneePos = HipPos + RotatedDir * UpperLen;

	
		//FVector DirToTarget = RootToEff.GetSafeNormal();
		//FVector PlaneNormal = FVector::CrossProduct(DirToTarget, (Pole - HipPos)).GetSafeNormal();
	//THIS WAS CHANGED
		//FVector RotatedDir = DirToTarget.RotateAngleAxis(FMath::RadiansToDegrees(KneeAngle), PlaneNormal);
		//RotatedDir.Normalize();
		//RotatedDir.Z = -1.0f;
		//FVector NewKneePos = HipPos + RotatedDir  * UpperLen;
	
	// Set joint transforms
		//NewKneePos -= Knee->GetUpVector() * 20;
		Knee->SetWorldLocation(NewKneePos);
		Foot->SetWorldLocation(Target);
	
	// Orient joints
		//FRotator HipRot = UKismetMathLibrary::FindLookAtRotation(HipPos, NewKneePos);
		//FRotator KneeRot = UKismetMathLibrary::FindLookAtRotation(NewKneePos, Target);
		//FRotator AxisCorrection(0.f, 90.f, 0.f); // example: rotate +90° around yaw if your mesh points +Y instead of +X
		//HipRot += AxisCorrection;
		//KneeRot += AxisCorrection;
		
		
		//Hip->SetWorldRotation(HipRot);
		//Knee->SetRelativeRotation(KneeRot);
		//Foot->SetRelativeRotation(TargetPose.Rotation.Rotator());

		DrawDebugSphere(GetWorld(), HipPos, 8.f, 12, FColor::Red);
		DrawDebugSphere(GetWorld(), NewKneePos, 8.f, 12, FColor::Green);
	
		DrawDebugLine(GetWorld(), HipPos, NewKneePos, FColor::Red);
		DrawDebugLine(GetWorld(), NewKneePos, Target, FColor::Green);
		DrawDebugSphere(GetWorld(), Target, 5.f, 8, FColor::Yellow);
	
		DrawDebugLine(GetWorld(), HipPos, HipPos + PlaneNormal * 50.f, FColor::Blue);
		DrawDebugLine(GetWorld(), HipPos, HipPos + DirToTarget * 50.f, FColor::Cyan);

		DrawDebugLine(GetWorld(), HipPos, HipPos + ForwardPoleDir * 50.f, FColor::Purple);
		DrawDebugLine(GetWorld(), HipPos, HipPos + PlaneNormal * 50.f, FColor::Orange);
}

FVector ULeg::FootBottom() const
{
	if (Bones.Num() < 3 || !Bones[2]) return FVector::ZeroVector;
	return Bones[2]->GetComponentLocation() + FVector::DownVector * GroundOffset;
}
