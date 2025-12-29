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
	UActionAnimModelMovingMovement(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
	void Init(AMovingModel* InMovingModel);

private:
	AMovingModel* MovingModelRef;
};
