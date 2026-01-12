// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionAnimModelJumpingMovement.generated.h"

/**
* Picks up item from ground into hand
 */
UCLASS()
class SIDEPROJECT_API UActionAnimModelJumpingMovement : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionAnimModelJumpingMovement* Create(UObject* Outer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
};
