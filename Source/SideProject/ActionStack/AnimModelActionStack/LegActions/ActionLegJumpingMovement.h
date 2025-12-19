// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionLegJumpingMovement.generated.h"

/**
* Picks up item from ground into hand
 */
UCLASS()
class SIDEPROJECT_API UActionLegJumpingMovement : public UAction
{
	GENERATED_BODY()
	
public:
	UActionLegJumpingMovement(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation() override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
};
