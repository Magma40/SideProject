#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmMovingMovement.generated.h"

class UMoveArm;

UCLASS()
class SIDEPROJECT_API UActionArmMovingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	virtual void Init(UMoveArm* InMoveArm)
	{
		MoveArmRef = InMoveArm;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual bool IsDone() const override { 	return false; }
	
	bool ShouldHoldUpHands() const;
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
};
