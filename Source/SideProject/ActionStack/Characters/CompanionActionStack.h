#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CompanionActionStack.generated.h"

class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UCompanionActionStack : public UActionStack
{
	GENERATED_BODY()

	//FUNCTIONS
public:
	//Typed initialization called by the templated factory.
	void Init(ABaseInteractableCharacter* InOwnerCharacter)
	{
		OwnerCharacter = InOwnerCharacter;
	}
	
	//Actions
	void OnAddActionMoveTo(const FVector& InMoveToLocation);
	
	void OnAddActionPickUp();
	
	void OnAddActionContinuousMove(const FVector& InMoveToLocation);
	
	void OnAddTryToInteractWithWorld();
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	float MinDistanceForMoveTo = 10.0f;
	
	float MinDistanceForContinuousMoveTo = 25.0f;
};
