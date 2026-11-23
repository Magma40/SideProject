#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmToHolster.generated.h"

class UAnimationCurveSubsystem;
class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmToHolster : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		UMoveArm* InMoveArm, 
		USceneComponent* InBackHolster,
		UAnimationCurveSubsystem* InAnimSubsystem, 
		const float InPlayRate)
	{
		OwnerCharacter = InOwnerCharacter;
		MoveArmRef = InMoveArm;
		BackHolsterRef = InBackHolster;
		AnimSubsystem = InAnimSubsystem;
		PlayRate = InPlayRate;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual void OnEnd() override;
	
	virtual bool IsDone() const override;

	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> BackHolsterRef = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UAnimationCurveSubsystem> AnimSubsystem = nullptr;
	
	float PlayRate = 1.0f;
	
	UPROPERTY(Transient)
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
};
