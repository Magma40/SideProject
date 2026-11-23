#include "ActionStack/Characters/CompanionActions/ActionMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Player//BaseInteractableCharacter.h"

void UActionMoveTo::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	bMoveCompleted = false;
	
	check(IsValid(CompanionCharacter));
	
    AAIController* AIController = CompanionCharacter->GetController<AAIController>();
	check(IsValid(AIController));
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	check(IsValid(PathComp));

	PathComp->AbortMove(*AIController, FPathFollowingResultFlags::ForcedScript);
	PathComp->OnRequestFinished.AddUObject(this, &UActionMoveTo::OnMoveCompleted);

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(MinDistance);

	FNavPathSharedPtr NavPath;
	MoveRequestID = AIController->MoveTo(MoveRequest, &NavPath);
	
	if (MoveRequestID != EPathFollowingRequestResult::RequestSuccessful)
	{
		//Extra debug info
		UE_LOG(LogTemp, Error, TEXT("MoveTo FAILED! Pawn=%s Target=%s"), *CompanionCharacter->GetName(),*TargetLocation.ToString());
		
		UE_LOG(LogTemp, Warning, TEXT(" - Controller: %s"), *AIController->GetName());
		UE_LOG(LogTemp, Warning, TEXT(" - Pawn has NavAgentProperties? %s"), CompanionCharacter->GetNavAgentPropertiesRef().IsValid() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Warning, TEXT(" - PathFollowingComponent Valid: %s"),PathComp ? TEXT("YES") : TEXT("NO"));
		
		bMoveCompleted = true;
	}
}

bool UActionMoveTo::IsDone() const
{
	return bMoveCompleted;
}

void UActionMoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	bMoveCompleted = true;
	
	const AAIController* AIController = CompanionCharacter->GetController<AAIController>();
	check(IsValid(AIController));
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	check(IsValid(PathComp));
	
	PathComp->OnRequestFinished.RemoveAll(this);
}