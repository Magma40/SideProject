#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionAnimModelMovingMovement.generated.h"

class UMovingModel;

UCLASS()
class SIDEPROJECT_API UActionAnimModelMovingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(UMovingModel* InMovingModel)
	{
		MovingModel = InMovingModel;
	}
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual bool IsDone() const override { return false; }

	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<UMovingModel> MovingModel = nullptr;
};
