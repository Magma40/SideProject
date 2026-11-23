#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "ActionStack/ActionStack.h"
#include "ActionContinuousMoveTo.generated.h"

struct FPathFollowingResult;
struct FAIRequestID;
class ABaseInteractableCharacter;
class AAIController;

/**
 * Moves a CompanionCharacter to a target location using AIController, I had no idea this is how you write a summary in unreal
 */
UCLASS()
class SIDEPROJECT_API UActionContinuousMoveTo : public UAction
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	void Init(ABaseInteractableCharacter* InOwnerCharacter,
		const FVector& InTargetLocation,
		const float InMinFollowDistance)
	{
		CompanionCharacter = InOwnerCharacter;
		ToLocation = InTargetLocation;
		FollowDistance = InMinFollowDistance;
	}
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual bool IsDone() const override;
	
	void StartMove();
	
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	UPROPERTY(Transient)
	FVector ToLocation = FVector::ZeroVector;
	
	float FollowDistance = 25.0f;
	
	bool bIsMoving = false;
	
	UPROPERTY(Transient)
	FAIRequestID  MoveRequestID;
};
