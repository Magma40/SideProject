#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelJumpingMovement.h"
#include "AIController.h"
#include "Player/CompanionPawn.h"

UActionAnimModelJumpingMovement* UActionAnimModelJumpingMovement::Create(UObject* Outer, 
	ABaseInteractableCharacter* InOwnerCharacter, 
	ABaseInteractableCharacter* InOtherOwnerCharacter)
{
	UActionAnimModelJumpingMovement* Action = NewObject<UActionAnimModelJumpingMovement>(Outer);
	
	Action->OwnerCharacter = InOwnerCharacter;
	Action->OtherOwnerCharacter = InOtherOwnerCharacter;
	
	return Action;
}

void UActionAnimModelJumpingMovement::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelJumpingMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	
	if (Cast<ACompanionPawn>(OwnerCharacter))
	{
		const FVector PlayerDirVelocity = OtherOwnerCharacter->GetVelocity();
		
		OwnerCharacter->AddMovementInput(PlayerDirVelocity, DeltaTime);
		
		if (AAIController* AIController = OwnerCharacter->GetController<AAIController>())
		{
			AIController->StopMovement();
		}
	}
}

bool UActionAnimModelJumpingMovement::IsDone_Implementation() const
{
	return Super::IsDone_Implementation();
}
