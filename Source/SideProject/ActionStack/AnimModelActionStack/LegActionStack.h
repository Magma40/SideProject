#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "LegActionStack.generated.h"

class UMoveLeg;
class ABaseInteractableCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API ULegActionStack : public UActionStack
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	void Init(const TArray<UMoveLeg*>& InLegsRef, 
		ABaseInteractableCharacter* InOwnerCharacter);
	
	void AddMovingAction();
	
	void AddJumpingAction();
	
protected:
	virtual void BeginPlay() override;
	
	//FIELDS:
private:
	UPROPERTY()
	TArray<TObjectPtr<UMoveLeg>> LegsRef;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
};
