#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmPoint.generated.h"

class UAnimationCurveSubsystem;
class APointingIndicator;
class APlayerCamera;
class UMoveArm;
class ABaseInteractableCharacter;
class AInteractableItemBase;

UCLASS()
class SIDEPROJECT_API UActionArmPoint : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		UMoveArm* InMoveArm, 
		UAnimationCurveSubsystem* InAnimSubsystem,
		APlayerCamera* InPlayerCamera,
		const FVector& InPointingAtLocation,
		const float InPlayRate)
	{
		OwnerCharacter = InOwnerCharacter;
		MoveArmRef = InMoveArm;
		AnimSubsystem = InAnimSubsystem;
		PlayerCamera = InPlayerCamera;
		PointingAtLocation = InPointingAtLocation;
		PlayRate = InPlayRate;
	}
	
	virtual void OnUpdate(float DeltaTime) override;
	
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
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY(Transient)
	FVector PointingAtLocation = FVector::ZeroVector;
	
	float PlayRate = 1.0f;
	
	UPROPERTY(Transient)
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
};