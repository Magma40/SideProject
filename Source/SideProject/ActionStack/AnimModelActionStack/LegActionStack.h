// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "LegActionStack.generated.h"

class UMoveLeg;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API ULegActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	ULegActionStack();
	virtual void BeginPlay() override;
	
	TArray<UMoveLeg*> LegsRef;
};
