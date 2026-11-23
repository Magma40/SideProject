#include "ActionStack/AnimModelActionStack/LegActions/ActionLegMovingMovement.h"
#include "ProcedualAnimation/MoveLeg.h"

void UActionLegMovingMovement::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(!MoveLegsRef.IsEmpty());
	
	for (UMoveLeg* Leg : MoveLegsRef)
	{
		Leg->bIsJumping = false;
		Leg->bFootPlanted = true;
		Leg->bIsStepping = false;
	}
}

void UActionLegMovingMovement::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(!MoveLegsRef.IsEmpty());

	//Update each leg
	 for (UMoveLeg* Leg : MoveLegsRef)
	 {
	 	//If another leg is not stepping and current leg is able to step
	 	if (Leg->ShouldStep())
	 	{
	 		Leg->StartStep();
	 	}
	 	else
	 	{
	 		//Foot stays put while another leg steps
	 		const FTransform UpperLegXForm = Leg->LegParts[0]->GetComponentTransform();
	 		
	 		Leg->TargetLocal = UpperLegXForm.InverseTransformPosition(Leg->LockedFootPosition.Position);
	 	}
	 }
}