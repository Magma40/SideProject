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
	static UActionArmMovingMovement* Create(UObject* Outer,
		UMoveArm* InMoveArm);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	bool ShouldHoldUpHands() const;
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
};
