#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionPickUpItem.generated.h"

class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionPickUpItem : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionPickUpItem* Create(UObject* Outer, 
		ABaseInteractableCharacter* InCompanionCharacter);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> CompanionCharacter = nullptr;
	
	bool TriedToPickUpItem = false;
};
