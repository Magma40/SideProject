#include "ActionStack/Characters/CompanionActions/ActionContinuousMoveTo.h"
#include "Player/BaseInteractableCharacter.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

void UActionContinuousMoveTo::OnUpdate(const float DeltaTime)
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(CompanionCharacter));
	
	if (!bIsMoving)
	{
		const float DistSq = FVector::Dist(
			CompanionCharacter->GetActorLocation(),
			ToLocation);
	
		if (DistSq < FollowDistance)
		{
			return;
		}
		
		StartMove();
	}
}

bool UActionContinuousMoveTo::IsDone() const
{
	check(IsValid(CompanionCharacter));
	check(IsValid(CompanionCharacter->GetCharacterMovement()));
	
	return CompanionCharacter->GetCharacterMovement()->IsFalling();
}

void UActionContinuousMoveTo::StartMove()
{
	check(IsValid(CompanionCharacter));
	
	AAIController* AIController = CompanionCharacter->GetController<AAIController>();
	check(IsValid(AIController));
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	check(IsValid(PathComp));
	
	PathComp->AbortMove(*AIController, FPathFollowingResultFlags::ForcedScript);
	PathComp->OnRequestFinished.AddUObject(this, &UActionContinuousMoveTo::OnMoveCompleted);

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(ToLocation);
	MoveRequest.SetAcceptanceRadius(FollowDistance);

	FNavPathSharedPtr NavPath;
	MoveRequestID = AIController->MoveTo(MoveRequest, &NavPath);
	
	if (MoveRequestID != EPathFollowingRequestResult::RequestSuccessful)
	{
		//Extra debug info
		UE_LOG(LogTemp, Error, TEXT("MoveTo FAILED! Pawn=%s Target=%s"), *CompanionCharacter->GetName(),*ToLocation.ToString());
		
		UE_LOG(LogTemp, Warning, TEXT(" - Controller: %s"), *AIController->GetName());
		UE_LOG(LogTemp, Warning, TEXT(" - Pawn has NavAgentProperties? %s"), CompanionCharacter->GetNavAgentPropertiesRef().IsValid() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Warning, TEXT(" - PathFollowingComponent Valid: %s"),PathComp ? TEXT("YES") : TEXT("NO"));
	}
}

void UActionContinuousMoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	const AAIController* AIController = CompanionCharacter->GetController<AAIController>();
	check(IsValid(AIController));
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	check(IsValid(PathComp));
	
	PathComp->OnRequestFinished.RemoveAll(this);
}