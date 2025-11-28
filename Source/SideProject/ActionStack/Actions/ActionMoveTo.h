// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionMoveTo.generated.h"

class UCompanionCharacter;

UCLASS()
class SIDEPROJECT_API UActionMoveTo : public UAction
{
	GENERATED_BODY()

public:
	UActionMoveTo(const FObjectInitializer& ObjectInitializer);
	void Init(UCompanionCharacter* InCompanionCharacter, FVector InTargetLocation);
	void OnUpdate_Implementation() override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	UCompanionCharacter* CompanionCharacter = nullptr;
	FVector TargetLocation = FVector::ZeroVector;
	float MinDistance = 200.0f;
};
