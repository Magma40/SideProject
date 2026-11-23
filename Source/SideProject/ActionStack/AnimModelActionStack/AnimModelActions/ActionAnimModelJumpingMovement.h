#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionAnimModelJumpingMovement.generated.h"

class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionAnimModelJumpingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter)
	{
		OwnerCharacter = InOwnerCharacter;
	}
	
	virtual bool IsDone() const override;
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
};
