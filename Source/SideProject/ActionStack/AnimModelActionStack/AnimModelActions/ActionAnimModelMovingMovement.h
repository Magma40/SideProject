// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionAnimModelMovingMovement.generated.h"

class AMovingModel;


UCLASS()
class SIDEPROJECT_API UActionAnimModelMovingMovement : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionAnimModelMovingMovement* Create(UObject* Outer, AMovingModel* InMovingModel);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	AMovingModel* MovingModelRef = nullptr;
};
