// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "AnimModelActionStack.generated.h"

class UArmActionStack;
class ULegActionStack;
class AMovingModel;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UAnimModelActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	UAnimModelActionStack();
	virtual void BeginPlay() override;
	
	AMovingModel* MovingModelRef = nullptr;
	ULegActionStack* LegActionStackRef = nullptr;
	UArmActionStack* ArmActionStackRef = nullptr;

public:
	void PushDefaultActionMovement();
};
