// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmMovingMovement.generated.h"

class UMoveArm;
UCLASS()
class SIDEPROJECT_API UActionArmMovingMovement : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionArmMovingMovement* Create(UObject* Outer, UMoveArm* InMoveArm);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	UMoveArm* MoveArmRef = nullptr;
};
