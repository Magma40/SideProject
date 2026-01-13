// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/LegActions/ActionLegMovingMovement.h"
#include "ProcedualAnimation/MoveLeg.h"

UActionLegMovingMovement* UActionLegMovingMovement::Create(UObject* Outer, const TArray<UMoveLeg*>& InMoveLegs)
{
	UActionLegMovingMovement* Action = NewObject<UActionLegMovingMovement>(Outer);
	Action->MoveLegsRef = InMoveLegs;
	return Action;
}


void UActionLegMovingMovement::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = false;
		Leg->bFootPlanted = true;
		Leg->bIsStepping = false;

		// Leg->TargetPose.Position = Leg->LegParts[0]->GetComponentTransform().InverseTransformPosition(Leg->LegParts[2]->GetComponentLocation());
		// Leg->TargetPose.Rotation = Leg->LegParts[2]->GetComponentQuat();
		// Leg->LockedFootPosition.Position = Leg->TargetPose.Position;
		// Leg->LockedFootPosition.Rotation = Leg->TargetPose.Rotation;
	}
}

void UActionLegMovingMovement::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	 for (UMoveLeg* Leg : MoveLegsRef)
	 {
	 	if (Leg->ShouldStep())
	 	{
	 		Leg->StartStep();
	 	}
	 	else
	 	{
	 		const FTransform UpperLegXForm = Leg->LegParts[0]->GetComponentTransform();
	 		Leg->TargetLocal = UpperLegXForm.InverseTransformPosition(Leg->LockedFootPosition.Position);
	 	}
	 }
}

bool UActionLegMovingMovement::IsDone_Implementation() const
{
	return false;
}
