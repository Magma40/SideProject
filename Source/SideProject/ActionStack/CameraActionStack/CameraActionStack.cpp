#include "CameraActionStack.h"
#include "CameraPoint.h"
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Player/MainCharacter.h"
#include "WorldObjects/CameraManagerMain.h"
#include "WorldObjects/PlayerCamera.h"

void UCameraActionStack::PushDefaultActionMovement(const float InRotationLimit, const float InSliderMoveSpeed)
{
	check(IsValid(OwnerCharacter));
	check(IsValid(PlayerCamera));
	check(IsValid(PlayerController));
	check(IsValid(InputDeviceSubsystem));
	
	UActionCameraDefaultMovement* DefaultMovement = 
		CreateAction<UActionCameraDefaultMovement>(this, 
		OwnerCharacter, 
		PlayerCamera,
		PlayerController,
		InputDeviceSubsystem,
		InRotationLimit,
		InSliderMoveSpeed);
	
	PushAction(*DefaultMovement);
}

void UCameraActionStack::PushToCameraPointAction(ACameraPoint* InNewCameraPoint, const float InPlayRate)
{
	check(IsValid(PlayerCamera));
	check(IsValid(InNewCameraPoint));
	check(IsValid(AnimSubsystem));
	check(IsValid(CameraManager));
	
	UActionToCameraPoint* ToCameraPoint = 
		CreateAction<UActionToCameraPoint>(this, 
		PlayerCamera,
		InNewCameraPoint, 
		AnimSubsystem,
		CameraManager,
		InPlayRate);
	
	PushAction(*ToCameraPoint);
}