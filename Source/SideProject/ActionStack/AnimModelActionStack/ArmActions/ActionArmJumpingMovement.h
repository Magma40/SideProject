// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmJumpingMovement.generated.h"

class UMoveArm;
UCLASS()
class SIDEPROJECT_API UActionArmJumpingMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionArmJumpingMovement* Create(UObject* Outer,
		UMoveArm* InMoveArm);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
};
