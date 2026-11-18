#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionLegJumpingMovement.generated.h"

class ABaseInteractableCharacter;
class UMoveLeg;

UCLASS()
class SIDEPROJECT_API UActionLegJumpingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionLegJumpingMovement* Create(UObject* Outer, 
		const TArray<UMoveLeg*>& InMoveLegs,
		ABaseInteractableCharacter* InOwnerCharacter);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	virtual void OnEnd_Implementation() override;

	//FIELDS:
private:
	UPROPERTY()
	TArray<TObjectPtr<UMoveLeg>> MoveLegsRef;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
};
