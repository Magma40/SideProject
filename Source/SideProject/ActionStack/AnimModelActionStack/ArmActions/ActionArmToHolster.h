// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmToHolster.generated.h"

class UMoveArm;

UCLASS()
class SIDEPROJECT_API UActionArmToHolster : public UAction // Because that's what happens on the ....Polar..... Express!!!!!!
{
	GENERATED_BODY()
	
public:
	UActionArmToHolster(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

	void Init(UMoveArm* InMoveArm);

private:
	UMoveArm* MoveArmRef;
	UPROPERTY() UActionArmFromHolster* ArmsFromHolster = nullptr;
	UPROPERTY(EditDefaultsOnly, Category= "Animation") UCurveFloat* HolsterAnimCurve;
};
