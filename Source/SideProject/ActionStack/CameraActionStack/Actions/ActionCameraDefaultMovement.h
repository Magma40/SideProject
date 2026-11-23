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
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	void Init(AMainCharacter* InOwnerCharacter, 
		APlayerCamera* InPlayerCamera,
		APlayerController* InPlayerController,
		UInputDeviceSubsystem* InInputDeviceSubsystem,
		const float InRotationLimit,
		const float InSliderMoveSpeed)
	{
		OwnerCharacter = InOwnerCharacter;
		PlayerCamera = InPlayerCamera;
		PlayerController = InPlayerController;
		InputDeviceSubsystem = InInputDeviceSubsystem;
		RotationLimit = InRotationLimit;
		SliderMoveSpeed = InSliderMoveSpeed;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual bool IsDone() const override { return false; }
	
	void LimitLookRangeForCamera(const FInputActionValue& Value);
	
	void LimitMoveRangeForCamera();
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<AMainCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UInputDeviceSubsystem> InputDeviceSubsystem = nullptr;
	
	float RotationLimit = 45.0f;
	
	float SliderMoveSpeed = 100.0f;
};