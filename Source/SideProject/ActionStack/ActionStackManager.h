// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionStackManager.generated.h"

UCLASS()
class SIDEPROJECT_API AActionStackManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AActionStackManager();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere)
	UActionStack* ActionStack;
};
