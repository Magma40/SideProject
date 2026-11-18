#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "ActionStack/ActionStack.h"
#include "Navigation/PathFollowingComponent.h"
#include "ActionMoveTo.generated.h"

class ABaseInteractableCharacter;
class AMovingModel;

/**
 * Moves a CompanionCharacter to a target location using AIController, I had no idea this is how you write a summary in unreal
 */
UCLASS()
class SIDEPROJECT_API UActionMoveTo : public UAction
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	static UActionMoveTo* Create(UObject* Outer,
		ABaseInteractableCharacter* InCompanionCharacter, 
		const FVector& InTargetLocation, 
		bool InNeedIndicator);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnEnd_Implementation() override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;
	
	float MinDistance = 10.0f;
	
	UPROPERTY()
	TObjectPtr<AActor> IndicatorActor = nullptr;
	
	bool bNeedIndicator = true;
	
	bool bMoveCompleted = false;
	
	UPROPERTY()
	FAIRequestID  MoveRequestID;
};
