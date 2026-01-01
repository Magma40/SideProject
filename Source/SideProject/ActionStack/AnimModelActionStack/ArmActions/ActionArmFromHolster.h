// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmFromHolster.generated.h"

class UMoveArm;
UCLASS()
class SIDEPROJECT_API UActionArmFromHolster : public UAction
{
	GENERATED_BODY()
	
public:
	UActionArmFromHolster(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

	void Init(UMoveArm* InMoveArm);

private:
	UMoveArm* MoveArmRef;
};
