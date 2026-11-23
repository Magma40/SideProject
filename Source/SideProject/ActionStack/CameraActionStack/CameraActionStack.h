#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CameraActionStack.generated.h"

class ACameraManagerMain;
class UAnimationCurveSubsystem;
class APlayerCamera;
class AMainCharacter;
class ACameraPoint;
class APlayerController;
class UInputDeviceSubsystem;

UCLASS()
class SIDEPROJECT_API UCameraActionStack : public UActionStack
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	void Init(AMainCharacter* InOwnerCharacter,
		APlayerCamera* InPlayerCamera,
		APlayerController* InPlayerController,
		UInputDeviceSubsystem* InInputDeviceSubsystem,
		UAnimationCurveSubsystem* InAnimSubsystem,
		ACameraManagerMain* InCameraManager)
	{
		OwnerCharacter = InOwnerCharacter;
		PlayerCamera = InPlayerCamera;
		PlayerController = InPlayerController;
		InputDeviceSubsystem = InInputDeviceSubsystem;
		AnimSubsystem = InAnimSubsystem;
		CameraManager = InCameraManager;
	}
	
	void PushDefaultActionMovement(const float InRotationLimit, const float InSliderMoveSpeed);
	
	void PushToCameraPointAction(ACameraPoint* InNewCameraPoint, float InPlayRate);
	
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
	
	UPROPERTY(Transient)
	TObjectPtr<UAnimationCurveSubsystem> AnimSubsystem = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<ACameraManagerMain> CameraManager = nullptr;
};