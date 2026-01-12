// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/Actions/ActionContinuousMoveTo.h"
#include "Player/BaseInteractableCharacter.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

UActionContinuousMoveTo* UActionContinuousMoveTo::Create(UObject* Outer, ABaseInteractableCharacter* InCompanionCharacter, ABaseInteractableCharacter* InOtherCharacter)
{
	UActionContinuousMoveTo* Action = NewObject<UActionContinuousMoveTo>(Outer);
	Action->CompanionCharacter = InCompanionCharacter;
	Action->OtherCharacter = InOtherCharacter;
	return Action;
}

void UActionContinuousMoveTo::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);

	AAIController* AIController = CompanionCharacter->GetController<AAIController>();
	if (!AIController) return;
	CachedController = AIController;

	GetWorld()->GetTimerManager().SetTimer(FollowTimer,this,&UActionContinuousMoveTo::StartMove,CheckInterval,true);
}

void UActionContinuousMoveTo::StartMove() const
{
	const float DistSq = FVector::DistSquared(CompanionCharacter->GetActorLocation(),OtherCharacter->GetActorLocation());
	if (DistSq <= FMath::Square(FollowDistance)) return;

	if (CachedController->GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Moving) return;
	
	UPathFollowingComponent* PathComp = CachedController->GetPathFollowingComponent();
	if (!PathComp) return;

	EPathFollowingRequestResult::Type Result = CachedController->MoveToActor(
		OtherCharacter,
		FollowDistance,   // small radius so it doesn't jitter
		false,
		true,
		true
	);

	if (Result != EPathFollowingRequestResult::RequestSuccessful)
	{
		// Extra debug info
		UE_LOG(LogTemp, Error, TEXT("MoveTo FAILED! Pawn=%s Target=%s"), *CompanionCharacter->GetName(), *OtherCharacter->GetName());
		
		UE_LOG(LogTemp, Warning, TEXT(" - Controller: %s"), *CachedController->GetName());
		UE_LOG(LogTemp, Warning, TEXT(" - Pawn has NavAgentProperties? %s"), CompanionCharacter->GetNavAgentPropertiesRef().IsValid() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Warning, TEXT(" - PathFollowingComponent Valid: %s"),PathComp ? TEXT("YES") : TEXT("NO"));
	}
}

void UActionContinuousMoveTo::OnUpdate_Implementation(float DeltaTime)
{
	Super::OnUpdate_Implementation(DeltaTime);
	if (OtherCharacter->Sprinting) CompanionCharacter->GetCharacterMovement()->MaxWalkSpeed = CompanionCharacter->MaxSprintSpeed;
	else  CompanionCharacter->GetCharacterMovement()->MaxWalkSpeed = CompanionCharacter->WalkSpeed;
}

bool UActionContinuousMoveTo::IsDone_Implementation() const
{
	return false;
}

void UActionContinuousMoveTo::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	GetWorld()->GetTimerManager().ClearTimer(FollowTimer);
	CachedController->StopMovement();
}

