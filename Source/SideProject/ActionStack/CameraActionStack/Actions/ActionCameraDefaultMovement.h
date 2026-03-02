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

	void LimitLookRangeForCamera(const FInputActionValue& Value);
	void LimitMoveRangeForCamera();

	void SetRotationLimit(const float InRotationLimit) { RotationLimit = InRotationLimit; }
	
private:
	AMainCharacter* OwnerCharacter = nullptr;
	APlayerCamera* PlayerCamera = nullptr;
	float RotationLimit = 45.0f;
	float SliderMoveSpeed = 100.0f;
};
