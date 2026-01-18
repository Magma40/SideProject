// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionToCameraPoint.generated.h"

class APlayerCamera;

UCLASS()
class SIDEPROJECT_API UActionToCameraPoint : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionToCameraPoint* Create(UObject* Outer, APlayerCamera* InPlayerCamera);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
	
private:
	APlayerCamera* PlayerCamera = nullptr;
	TArray<FTransform> AnimTransforms;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;
};
