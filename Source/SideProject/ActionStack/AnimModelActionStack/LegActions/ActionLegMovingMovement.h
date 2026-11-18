#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionLegMovingMovement.generated.h"

class UMoveLeg;

UCLASS()
class SIDEPROJECT_API UActionLegMovingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionLegMovingMovement* Create(UObject* Outer,
		const TArray<UMoveLeg*>& InMoveLegs);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	//FIELDS:
private:
	UPROPERTY()
	TArray<TObjectPtr<UMoveLeg>> MoveLegsRef;
};
