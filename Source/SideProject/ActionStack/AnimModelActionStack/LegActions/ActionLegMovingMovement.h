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
	//Typed initialization called by the templated factory.
	virtual void Init(const TArray<UMoveLeg*>& InMoveLegs)
	{
		MoveLegsRef = InMoveLegs;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual bool IsDone() const override { return false; }
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMoveLeg>> MoveLegsRef;
};
