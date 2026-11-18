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
	static UActionAnimModelJumpingMovement* Create(UObject* Outer, 
		ABaseInteractableCharacter* InOwnerCharacter, 
		ABaseInteractableCharacter* InOtherOwnerCharacter);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OtherOwnerCharacter = nullptr;
};
