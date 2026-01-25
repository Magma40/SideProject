// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmHandToLocation.generated.h"

class UMoveArm;

UCLASS()
class SIDEPROJECT_API UActionArmHandToLocation : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionArmHandToLocation* Create(UObject* Outer, UMoveArm* InMoveArm, const FVector& InToLocation);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
	
private:
	UMoveArm* MoveArmRef = nullptr;
	TArray<FTransform> AnimTransforms;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;

	FVector ToLocation = FVector::ZeroVector;
};
