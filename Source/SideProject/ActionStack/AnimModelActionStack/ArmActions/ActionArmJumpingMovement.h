#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmJumpingMovement.generated.h"

class ABaseInteractableCharacter;
class UMoveArm;

UCLASS()
class SIDEPROJECT_API UActionArmJumpingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		UMoveArm* InMoveArm)
	{
		OwnerCharacter = InOwnerCharacter;
		MoveArmRef = InMoveArm;
	}
	
	virtual bool IsDone() const override;
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
};
