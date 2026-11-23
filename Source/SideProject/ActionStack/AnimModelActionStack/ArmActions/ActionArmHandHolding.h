#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmHandHolding.generated.h"

class UAnimationCurveSubsystem;
class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmHandHolding : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		UMoveArm* InMoveArm, 
		UAnimationCurveSubsystem* InAnimSubsystem,
		const FVector& InOtherHandLocation,
		const float InPlayRate)
	{
		OwnerCharacter = InOwnerCharacter;
		MoveArmRef = InMoveArm;
		AnimSubsystem = InAnimSubsystem;
		ToLocation = InOtherHandLocation;
		PlayRate = InPlayRate;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual void OnEnd() override;
	
	virtual bool IsDone() const override { return false; }
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UAnimationCurveSubsystem> AnimSubsystem = nullptr;
	
	UPROPERTY(Transient)
	FVector ToLocation = FVector::ZeroVector;
	
	float PlayRate = 0.1f;
	
	UPROPERTY(Transient)
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
};