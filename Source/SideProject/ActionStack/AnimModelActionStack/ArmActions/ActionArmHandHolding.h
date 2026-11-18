#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmHandHolding.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmHandHolding : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionArmHandHolding* Create(UObject* Outer,
		UMoveArm* InMoveArm, 
		UMoveArm* InOtherMoveArm, 
		ABaseInteractableCharacter* InCharacter, 
		ABaseInteractableCharacter* InAICharacter);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	virtual void OnEnd_Implementation() override;
	
	//FIELDS:
public:
	UPROPERTY()
	FVector PointingLocation = FVector::ZeroVector;
	
	UPROPERTY()
	TObjectPtr<UActorComponent> ActorPointedAt = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveOtherArmRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> AIOwnerCharacter = nullptr;
	
	UPROPERTY()
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 0.1f;
};
