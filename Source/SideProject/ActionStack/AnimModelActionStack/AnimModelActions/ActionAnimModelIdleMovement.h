// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionAnimModelIdleMovement.generated.h"

/**
* Picks up item from ground into hand
 */
UCLASS()
class SIDEPROJECT_API UActionAnimModelIdleMovement : public UAction
{
	GENERATED_BODY()
	
public:
	UActionAnimModelIdleMovement(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
};
