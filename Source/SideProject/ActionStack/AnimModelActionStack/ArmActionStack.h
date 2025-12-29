// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class UMoveArm;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
protected:
	UArmActionStack();
	virtual void BeginPlay() override;
	
	TArray<UMoveArm*> ArmsRef;

};
