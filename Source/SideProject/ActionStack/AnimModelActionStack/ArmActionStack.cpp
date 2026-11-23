#include "ActionStack/AnimModelActionStack/ArmActionStack.h"

#include "AnimModelActionStack.h"
#include "ArmActions/ActionArmMovingMovement.h"
#include "ArmActions/ActionArmJumpingMovement.h"
#include "ArmActions/ActionArmToHolster.h"
#include "ArmActions/ActionArmWave.h"
#include "ArmActions/ActionArmPoint.h"
#include "ArmActions/ActionArmHandHolding.h"
#include "ArmActions/ActionArmHandToLocation.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MoveArm.h"
#include "WorldObjects/PlayerCamera.h"

void UArmActionStack::AddMovingAction()
{
	check(IsValid(Arm));
	
	UActionArmMovingMovement* ArmsDefaultMovement = 
		CreateAction<UActionArmMovingMovement>(this, 
		Arm);
	
	PushAction(*ArmsDefaultMovement);
}

void UArmActionStack::AddJumpingAction()
{
	check(IsValid(OwnerCharacter));
	check(IsValid(Arm));
	
	UActionArmJumpingMovement* ArmsJumpingMovement = 
		CreateAction<UActionArmJumpingMovement>(this, 
		OwnerCharacter,
		Arm);
	
	PushAction(*ArmsJumpingMovement);
}

void UArmActionStack::AddHolsterAction()
{
	check(IsValid(OwnerCharacter));
	check(IsValid(Arm));
	check(IsValid(BackHolsterRef));
	check(IsValid(AnimSubsystem));

	UActionArmToHolster* ArmsToHolster = 
		CreateAction<UActionArmToHolster>(this, 
		OwnerCharacter,
		Arm, 
		BackHolsterRef, 
		AnimSubsystem,
		HolsterPlayRate);
	
	PushAction(*ArmsToHolster);
}

void UArmActionStack::AddWaveAction()
{
	check(IsValid(OwnerCharacter));
	check(IsValid(Arm));
	check(IsValid(AnimSubsystem));

	UActionArmWave* ArmsWave = 
		CreateAction<UActionArmWave>(this, 
		OwnerCharacter,
		Arm, 
		AnimSubsystem,
		WavePlayRate);
	
	PushAction(*ArmsWave);
}

void UArmActionStack::AddPointingAction(const FVector& InPointingAtLocation)
{
	check(IsValid(OwnerCharacter));
	check(IsValid(Arm));
	check(IsValid(AnimSubsystem));
	check(IsValid(PlayerCamera));
	
	UActionArmPoint* ArmsPoint = 
		CreateAction<UActionArmPoint>(this,
		OwnerCharacter, 
		Arm,
		AnimSubsystem,
		PlayerCamera,
		InPointingAtLocation,
		PointPlayRate);
	
	PushAction(*ArmsPoint);
}

void UArmActionStack::AddHandHoldingAction(const FVector& InOtherHandLocation)
{
	check(IsValid(OwnerCharacter));
	check(IsValid(Arm));
	check(IsValid(AnimSubsystem));
	
	UActionArmHandHolding* ArmsHandHolding = 
		CreateAction<UActionArmHandHolding>(this, 
		OwnerCharacter,
		Arm,
		AnimSubsystem, 
		InOtherHandLocation, 
		HandHoldingPlayRate);
	
	PushAction(*ArmsHandHolding);
}

void UArmActionStack::AddHandToLocation( const FVector& InToLocation)
{
	check(IsValid(OwnerCharacter));
	check(IsValid(Arm));
	check(IsValid(AnimSubsystem));
	
	UActionArmHandToLocation* HandToLocation = 
		CreateAction<UActionArmHandToLocation>(this, 
		OwnerCharacter, 
		Arm,
		AnimSubsystem, 
		InToLocation,
		HandToLocationPlayRate);
	
	PushAction(*HandToLocation);
}

void UArmActionStack::HandlePointingAtAction()
{
	// const FVector PointingLocationRef = PointAction->GetPointingLocation();
	// 	
	// if (IsValid(PointAction->GetActorPointedAt()) && 
	// 	Cast<AInteractableItemBase>(PointAction->GetActorPointedAt()))
	// {
	// 	const AInteractableItemBase* Interaction = Cast<AInteractableItemBase>(PointAction->GetActorPointedAt());
	// 		
	// 	OwnerCompanion->GetCompanionActionStack()->OnAddActionPickUp();
	// 	OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(Interaction->GetActorLocation(), false);
	// }
	// else if (IsValid(PointAction->GetActorPointedAt()) && 
	// 		Cast<UInteractionBehaviour>(GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(PointAction->GetActorPointedAt())))
	// {
	// 	const UInteractionBehaviour* Interaction = Cast<UInteractionBehaviour>( GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(PointAction->GetActorPointedAt()));
	// 		
	// 	OwnerCompanion->GetCompanionActionStack()->OnAddTryToInteractWithWorld();
	// 	OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(Interaction->GetComponentLocation(), false);
	// 		
	// 	Cast<ACompanionPawn>(OwnerCompanion)->SetLastKnownInteractionBehaviour(Cast<UInteractionBehaviour>( GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(PointAction->GetActorPointedAt())));
	// }
	// else
	// {
	// 	OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(PointingLocationRef, true);
	
	//OwnerCompanion->GetCompanionActionStack()->OnAddActionMoveTo(OwnerCharacter->GetActorLocation(), false);

	// }
}