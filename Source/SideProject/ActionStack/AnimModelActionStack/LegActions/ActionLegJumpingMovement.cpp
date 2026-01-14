// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/LegActions/ActionLegJumpingMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MoveLeg.h"

UActionLegJumpingMovement* UActionLegJumpingMovement::Create(UObject* Outer, const TArray<UMoveLeg*>& InMoveLegs, ABaseInteractableCharacter* InOwnerCharacter)
{
	UActionLegJumpingMovement* Action = NewObject<UActionLegJumpingMovement>(Outer);
	Action->MoveLegsRef = InMoveLegs;
	Action->OwnerCharacter = InOwnerCharacter;
	return Action;
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
		const float Rise = FMath::Min(Leg->AirElapsedTime * Leg->AirRiseSpeed, Leg->MaxAirRise);
		FVector Goal = Leg->StepTargetPose.Position + FVector(0, 0, Rise);
		// Smooth blend
		Leg->TargetLocal = FMath::VInterpTo(Leg->TargetLocal, Goal, DeltaTime, 10.f);
		Leg->LockedFootPosition.Position = Leg->LegParts[0]->GetComponentTransform().TransformPosition(Leg->TargetPose.Position);
	}
}

void UActionLegJumpingMovement::OnEnd_Implementation()
{
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = false;
		Leg->bFootPlanted = true;
	}
}

bool UActionLegJumpingMovement::IsDone_Implementation() const
{
	if (OwnerCharacter->GetMovementComponent()->IsFalling()) return false;
	return true;
}
