#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "AnimModelActionStack.generated.h"

class ABaseInteractableCharacter;
class UArmActionStack;
class ULegActionStack;
class AMovingModel;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UAnimModelActionStack : public UActionStack
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	void PushDefaultActionMovement();
	
	void PushJumpingActionMovement();
	
	void PushHolsterActionMovement() const;
	
	void PushWaveActionMovement() const;
	
	void PushPointingActionMovement() const;
	
	void PushHandHoldingActionMovement();

	void RemovePointingActionMovement() const;
	
	//~ Begin Getters & Setters
	UFUNCTION(BlueprintPure) 
	UArmActionStack* GetArmActionStack_R() const
	{
		return ArmActionStack_RRef;
	}
	
	UFUNCTION(BlueprintPure)
	UArmActionStack* GetArmActionStack_L() const 
	{
		return ArmActionStack_LRef; 
	}
	
	UFUNCTION(BlueprintPure) 
	ULegActionStack* GetLegsActionStack() const
	{
		return LegActionStackRef;
	}
	//~ End Getters & Setters

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void Init();
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<AMovingModel> MovingModelRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> CompanionOwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ULegActionStack> LegActionStackRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<UArmActionStack> ArmActionStack_RRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<UArmActionStack> ArmActionStack_LRef = nullptr;

	float MinDistanceToHoldHands = 200.0f;
};
