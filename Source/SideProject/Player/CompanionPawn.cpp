#include "Player/CompanionPawn.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "Tools/GenericCalls.h"
#include "WorldInteractions/InteractionManager.h"

void ACompanionPawn::RespawnCharacter()
{
	Super::RespawnCharacter();
}

void ACompanionPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// //Get CompanionActionStack
	// CompanionActionStack = Cast<UCompanionActionStack>(GenericCalls::FindChildByExactName(this, TEXT("CompanionActionStack")));
	//
	// if (!CompanionActionStack)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - CompanionActionStack not found or wrong type"), *StaticClass()->GetName());
	// 	return;
	// }
}

//The Companion AI tries to run last known interaction
void ACompanionPawn::RunLastKnowInteract() const
{
	if (!IsValid(LastKnownInteractionBehaviour))
	{
		return;
	}
	
	CompanionActionStack->OnAddTryToInteractWithWorld();
	
	CompanionActionStack->OnAddActionMoveTo(LastKnownInteractionBehaviour->GetOwner()->GetActorLocation());
}