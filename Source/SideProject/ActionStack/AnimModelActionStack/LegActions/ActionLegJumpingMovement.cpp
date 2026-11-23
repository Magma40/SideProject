#include "ActionStack/AnimModelActionStack/LegActions/ActionLegJumpingMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MoveLeg.h"

void UActionLegJumpingMovement::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(!MoveLegsRef.IsEmpty());
	
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = true;
		Leg->bFootPlanted = false;
		Leg->bIsStepping = false;
		Leg->AirElapsedTime = 0;
		
		//Freeze the current foot
		Leg->LockedFootPosition.Position = Leg->LegParts[0]->GetComponentTransform().TransformPosition(Leg->TargetPose.Position);
	}
}

void UActionLegJumpingMovement::OnUpdate(float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(!MoveLegsRef.IsEmpty());
	
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->AirElapsedTime += DeltaTime;
		const float Rise = FMath::Min(Leg->AirElapsedTime * Leg->AirRiseSpeed, Leg->MaxAirRise);
		FVector Goal = Leg->StepTargetPose.Position + FVector(0, 0, Rise);
		
		//Smooth blend
		Leg->TargetLocal = FMath::VInterpTo(Leg->TargetLocal, Goal, DeltaTime, 10.f);
		Leg->LockedFootPosition.Position = Leg->LegParts[0]->GetComponentTransform().TransformPosition(Leg->TargetPose.Position);
	}
}

void UActionLegJumpingMovement::OnEnd()
{
	check(!MoveLegsRef.IsEmpty());
	
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = false;
		Leg->bFootPlanted = true;
	}
}

bool UActionLegJumpingMovement::IsDone() const
{
	check(IsValid(OwnerCharacter));
	check(IsValid(OwnerCharacter->GetMovementComponent()));
	
	return !OwnerCharacter->GetMovementComponent()->IsFalling();
}
