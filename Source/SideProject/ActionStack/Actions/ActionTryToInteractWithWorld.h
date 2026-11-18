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
	static UActionTryToInteractWithWorld* Create(UObject* Outer, 
		ABaseInteractableCharacter* InCompanionCharacter);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	bool TriedToInteractWithWorld = false;
};
