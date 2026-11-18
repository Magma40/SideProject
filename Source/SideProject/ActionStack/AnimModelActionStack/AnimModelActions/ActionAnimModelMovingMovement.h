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
	
	//FUNCTIONS:
public:
	static UActionAnimModelMovingMovement* Create(UObject* Outer, 
		AMovingModel* InMovingModel);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<AMovingModel> MovingModelRef = nullptr;
};
