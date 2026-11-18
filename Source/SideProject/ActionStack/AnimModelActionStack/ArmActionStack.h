#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;
class UInteractionBehaviour;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	void Init(UMoveArm*  InArmRef, 
		ABaseInteractableCharacter* InOwnerCharacter,  
		ABaseInteractableCharacter* InOwnerCompanion, 
		USceneComponent* InBackHolster);
	
	void AddMovingAction();
	
	void AddJumpingAction();
	
	void AddHolsterAction();
	
	void AddWaveAction();
	
	void AddPointingAction();
	
	void AddHandHoldingAction(UMoveArm* OtherCompanionArm);
	
	void AddHandToLocation(const FVector& InToLocation, bool bWantToPickUpItem);

	void RemovePointingAction();
	
	//~ Begin Getters & Setters
	UMoveArm* GetArm() const
	{
		return ArmRef;
	}
	//~ End Getters & Setters

protected:
	virtual void BeginPlay() override;
	
	//FIELDS:
private:
	UPROPERTY()
 	TObjectPtr<UMoveArm> ArmRef = nullptr;
	
	UPROPERTY() 
	TObjectPtr<USceneComponent> BackHolsterRef = nullptr;
	
	UPROPERTY() 
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY() 
	TObjectPtr<ABaseInteractableCharacter> OwnerCompanion = nullptr;
};
