#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmHandToLocation.generated.h"

class UAnimationCurveSubsystem;
class APlayerCamera;
class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmHandToLocation : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		UMoveArm* InMoveArm, 
		UAnimationCurveSubsystem* InAnimSubsystem,
		const FVector& InToLocation,
		const float InPlayRate)
	{
		OwnerCharacter = InOwnerCharacter;
		MoveArmRef = InMoveArm;
		AnimSubsystem = InAnimSubsystem;
		ToLocation = InToLocation;
		PlayRate = InPlayRate;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual bool IsDone() const override;
	
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
	
	float PlayRate = 1.0f;
	
	UPROPERTY(Transient)
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
};
