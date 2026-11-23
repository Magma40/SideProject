#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionTryToInteractWithWorld.generated.h"

class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionTryToInteractWithWorld : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	void Init(ABaseInteractableCharacter* InOwnerCharacter)
	{
		CompanionCharacter = InOwnerCharacter;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual bool IsDone() const override;
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	bool TriedToInteractWithWorld = false;
};
