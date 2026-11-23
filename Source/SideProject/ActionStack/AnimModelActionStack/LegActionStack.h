#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "LegActionStack.generated.h"

class UMoveLeg;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API ULegActionStack : public UActionStack
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	void Init(ABaseInteractableCharacter* InOwnerCharacter, 
		const TArray<UMoveLeg*>& InLegs)
	{
		OwnerCharacter = InOwnerCharacter;
		Legs = InLegs;
	}
	
	//Actions:
	void AddMovingAction();
	
	void AddJumpingAction();
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMoveLeg>> Legs;
	
	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
};
