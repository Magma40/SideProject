#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "ActionStack/ActionStack.h"
#include "Navigation/PathFollowingComponent.h"
#include "ActionMoveTo.generated.h"

class ABaseInteractableCharacter;

/**
 * Moves a CompanionCharacter to a target location using AIController, I had no idea this is how you write a summary in unreal
 */
UCLASS()
class SIDEPROJECT_API UActionMoveTo : public UAction
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	void Init(ABaseInteractableCharacter* InOwnerCharacter,
		const FVector& InTargetLocation,
		const float InMinDistance)
	{
		CompanionCharacter = InOwnerCharacter;
		TargetLocation = InTargetLocation;
		MinDistance = InMinDistance;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual bool IsDone() const override;
	
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	UPROPERTY(Transient)
	FVector TargetLocation = FVector::ZeroVector;
	
	float MinDistance = 10.0f;
	
	bool bMoveCompleted = false;
	
	UPROPERTY(Transient)
	FAIRequestID  MoveRequestID;
};
