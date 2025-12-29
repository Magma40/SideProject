// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/LegActions/ActionLegJumpingMovement.h"
#include "ProcedualAnimation/MoveLeg.h"

UActionLegJumpingMovement::UActionLegJumpingMovement(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionLegJumpingMovement::Init(TArray<UMoveLeg*> InMoveLegs)
{
	MoveLegsRef = InMoveLegs;
}

void UActionLegJumpingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = true;
		Leg->bFootPlanted = false;
		Leg->bIsStepping = false;
		Leg->AirElapsedTime = 0;
		// Freeze the current foot
		Leg->LockedFootPosition.Position = Leg->LegParts[0]->GetComponentTransform().TransformPosition(Leg->TargetPose.Position);
	}
}

void UActionLegJumpingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->AirElapsedTime += DeltaTime;
		float Rise = FMath::Min(Leg->AirElapsedTime * Leg->AirRiseSpeed, Leg->MaxAirRise);
		FVector Goal = Leg->StepTargetPose.Position + FVector(0, 0, Rise);
		// Smooth blend
		Leg->TargetLocal = FMath::VInterpTo(Leg->TargetLocal, Goal, DeltaTime, 10.f);
		Leg->LockedFootPosition.Position = Leg->LegParts[0]->GetComponentTransform().TransformPosition(Leg->TargetPose.Position);
	}
}

bool UActionLegJumpingMovement::IsDone_Implementation() const
{
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = false;
		Leg->bFootPlanted = true;
	}
	return Super::IsDone_Implementation();
}
