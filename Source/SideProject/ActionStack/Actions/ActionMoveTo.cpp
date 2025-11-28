// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Actions/ActionMoveTo.h"
#include "AIController.h"
#include "VectorTypes.h"
#include "ActionStack/Characters/CompanionCharacter.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"  

UActionMoveTo::UActionMoveTo(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionMoveTo::Init(UCompanionCharacter* InCompanionCharacter, FVector InTargetLocation)
{
	CompanionCharacter = InCompanionCharacter;
	TargetLocation = InTargetLocation;
}

void UActionMoveTo::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!CompanionCharacter) return;
	
	APawn* Pawn = CompanionCharacter->GetOwnerCharacter();
	if (!Pawn) return;
	
    AAIController* AIController = Pawn->GetController<AAIController>();
	if (!AIController) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(MinDistance);

	FNavPathSharedPtr NavPath;
	AIController->MoveTo(MoveRequest, &NavPath);
}


void UActionMoveTo::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
}

bool UActionMoveTo::IsDone_Implementation() const
{
	if (!CompanionCharacter) return true;
	APawn* Pawn = CompanionCharacter->GetOwnerCharacter();
	if (!Pawn) return true;
	
	if (FVector::Dist( Pawn->GetActorLocation(), TargetLocation) <= MinDistance) return true;
	
	AAIController* AIController = Pawn->GetController<AAIController>();
	if (AIController && AIController->GetMoveStatus() == EPathFollowingStatus::Idle) return true;

	return false;
}