#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class APlayerCamera;
class UAnimationCurveSubsystem;
class UMoveArm;
class ABaseInteractableCharacter;
class UInteractionBehaviour;

UCLASS()
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,  
		ABaseInteractableCharacter* InOwnerCompanion,
		UMoveArm* InArm, 
		USceneComponent* InBackHolster,
		UAnimationCurveSubsystem* InAnimSubsystem,
		APlayerCamera* InPlayerCamera,
		const float InHolsterPlayRate,
		const float InWavePlayRate,
		const float InPointPlayRate,
		const float InHandHoldingPlayRate,
		const float InHandToLocationPlayRate)
	{
		OwnerCharacter = InOwnerCharacter;
		OwnerCompanion = InOwnerCompanion;
		Arm = InArm;
		BackHolsterRef = InBackHolster;
		AnimSubsystem = InAnimSubsystem;
		PlayerCamera = InPlayerCamera;
		HolsterPlayRate = InHolsterPlayRate;
		WavePlayRate = InWavePlayRate;
		PointPlayRate = InPointPlayRate;
		HandHoldingPlayRate = InHandHoldingPlayRate;
		HandToLocationPlayRate = InHandToLocationPlayRate;
	}
	
	//Actions:
	void AddMovingAction();
	
	void AddJumpingAction();
	
	void AddHolsterAction();
	
	void AddWaveAction();
	
	void AddPointingAction(const FVector& InPointingAtLocation);
	
	void AddHandHoldingAction(const FVector& InOtherHandLocation);
	
	void AddHandToLocation(const FVector& InToLocation);
	
	void HandlePointingAtAction();
	
	//FIELDS:
private:
	UPROPERTY(Transient) 
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient) 
	TObjectPtr<ABaseInteractableCharacter> OwnerCompanion = nullptr;
	
	UPROPERTY(Transient)
 	TObjectPtr<UMoveArm> Arm = nullptr;
	
	UPROPERTY(Transient) 
	TObjectPtr<USceneComponent> BackHolsterRef = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UAnimationCurveSubsystem> AnimSubsystem = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	float HolsterPlayRate = 1.0f;
	
	float WavePlayRate = 1.0f;
	
	float PointPlayRate = 1.0f;
	
	float HandHoldingPlayRate = 1.0f;
	
	float HandToLocationPlayRate = 1.0f;
};