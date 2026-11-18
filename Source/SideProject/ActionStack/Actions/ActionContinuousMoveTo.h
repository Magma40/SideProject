#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionContinuousMoveTo.generated.h"

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
	static UActionContinuousMoveTo* Create(UObject* Outer, 
		ABaseInteractableCharacter* InCompanionCharacter,
		ABaseInteractableCharacter* InOtherCharacter);
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnEnd_Implementation() override;
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	void StartMove() const;

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OtherCharacter = nullptr;

	UPROPERTY()
	TWeakObjectPtr<AAIController> CachedController;
	
	UPROPERTY()
	FTimerHandle FollowTimer;
	
	float FollowDistance = 25.0f;
	
	float CheckInterval = 0.1f;
};
