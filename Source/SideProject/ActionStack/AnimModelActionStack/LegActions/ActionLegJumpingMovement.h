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
	//Typed initialization called by the templated factory.
	virtual void Init(ABaseInteractableCharacter* InOwnerCharacter,
		const TArray<UMoveLeg*>& InMoveLegs)
	{
		OwnerCharacter = InOwnerCharacter;
		MoveLegsRef = InMoveLegs;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual void OnEnd() override;
	
	virtual bool IsDone() const override;

	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMoveLeg>> MoveLegsRef;
};
