// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionCameraDefaultMovement.generated.h"

struct FInputActionValue;
class AMainCharacter;
class APlayerCamera;

UCLASS()
class SIDEPROJECT_API UActionCameraDefaultMovement : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionCameraDefaultMovement* Create(UObject* Outer, AMainCharacter* InOwnerCharacter, 	APlayerCamera* InPlayerCamera);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	void LimitLookRangeForCamera(const FInputActionValue& Value);
	void LimitMoveRangeForCamera(const FInputActionValue& Value);
	
private:
	AMainCharacter* OwnerCharacter = nullptr;
	APlayerCamera* PlayerCamera = nullptr;
	float RotationLimit = 45.0f;
	float MinSliderValue = -1000.0f;
	float MaxSliderValue = 1000.0f;
	float SliderMoveSpeed = 100.0f;
};
