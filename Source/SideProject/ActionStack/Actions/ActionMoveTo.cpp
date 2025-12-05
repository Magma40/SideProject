// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Actions/ActionMoveTo.h"
#include "ActionStack/Characters/CompanionCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "ProcedualAnimation/MovingModel.h"

UActionMoveTo::UActionMoveTo(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionMoveTo::Init(UCompanionCharacter* InCompanionCharacter, FVector InTargetLocation, AMovingModel* InMovingModel)
{
	CompanionCharacter = InCompanionCharacter;
	TargetLocation = InTargetLocation;
	MovingModel = InMovingModel;
}

void UActionMoveTo::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	bMoveCompleted = false;
	if (!CompanionCharacter) return;
	
	APawn* Pawn = CompanionCharacter->GetOwnerCharacter();
	if (!Pawn) return;
	
    AAIController* AIController = Pawn->GetController<AAIController>();
	if (!AIController) return;
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	if (!PathComp) return;

	PathComp->OnRequestFinished.AddUObject(this, &UActionMoveTo::OnMoveCompleted);
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(MinDistance);

	FNavPathSharedPtr NavPath;
	EPathFollowingRequestResult::Type Result = AIController->MoveTo(MoveRequest, &NavPath);
	if (Result != EPathFollowingRequestResult::RequestSuccessful)
	{
		// Extra debug info
		UE_LOG(LogTemp, Error, TEXT("MoveTo FAILED! Pawn=%s Target=%s"), *Pawn->GetName(),*TargetLocation.ToString());
		
		UE_LOG(LogTemp, Warning, TEXT(" - Controller: %s"), *AIController->GetName());
		UE_LOG(LogTemp, Warning, TEXT(" - Pawn has NavAgentProperties? %s"), Pawn->GetNavAgentPropertiesRef().IsValid() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Warning, TEXT(" - PathFollowingComponent Valid: %s"),PathComp ? TEXT("YES") : TEXT("NO"));
		
		bMoveCompleted = true;
	}
}


void UActionMoveTo::OnUpdate_Implementation() 
{
	Super::OnUpdate_Implementation();
	MovingModel->SetLegsMovingAnim();
}

bool UActionMoveTo::IsDone_Implementation() const
{
	if (!CompanionCharacter)
	{
		return true;
	}
	APawn* Pawn = CompanionCharacter->GetOwnerCharacter();
	if (!Pawn)
	{
		return true;
	}
	// float Distance = FVector::Dist( Pawn->GetActorLocation(), TargetLocation);
	// if (Distance <= MinDistance)
	// {
	// 	MovingModel->SetLegsMovingAnim();
	// 	return true;
	// }

	if (bMoveCompleted) MovingModel->SetLegsIdleAnim();
	return bMoveCompleted;
}

void UActionMoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	bMoveCompleted = true;

	// Cleanup delegate binding
	if (CompanionCharacter)
	{
		APawn* Pawn = CompanionCharacter->GetOwnerCharacter();
		if (Pawn)
		{
			if (AAIController* AIController = Pawn->GetController<AAIController>())
			{
				if (UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
				{
					PathComp->OnRequestFinished.RemoveAll(this);
				}
			}
		}
	}
}