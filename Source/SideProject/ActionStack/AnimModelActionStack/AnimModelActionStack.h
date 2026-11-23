#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "AnimModelActionStack.generated.h"

class ABaseInteractableCharacter;
class UArmActionStack;
class ULegActionStack;
class UMovingModel;

UCLASS()
class SIDEPROJECT_API UAnimModelActionStack : public UActionStack
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		UMovingModel* InMovingModel,
		ULegActionStack* InLegsActionStack,
		const TArray<UArmActionStack*> InArmsActionStack)
	{
		OwnerCharacter = InOwnerCharacter;
		MovingModel = InMovingModel;
		LegsActionStack = InLegsActionStack;
		ArmsActionStack = InArmsActionStack;
	}
	
	//Actions:
	void PushDefaultActionMovement();
	
	void PushJumpingActionMovement();
	
	void PushHolsterActionMovement() const;
	
	void PushWaveActionMovement() const;
	
	void PushPointingActionMovement(const FVector& InPointingAtLocation) const;
	
	void PushHandHoldingActionMovement_R(const FVector& InOtherHandLocation) const;
	
	void PushHandHoldingActionMovement_L(const FVector& InOtherHandLocation) const;
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UMovingModel> MovingModel = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<ULegActionStack> LegsActionStack = nullptr;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UArmActionStack>> ArmsActionStack;
};
