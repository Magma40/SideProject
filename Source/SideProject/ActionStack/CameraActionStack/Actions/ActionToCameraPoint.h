#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionToCameraPoint.generated.h"

class ACameraManagerMain;
class UAnimationCurveSubsystem;
class APlayerCamera;
class ACameraPoint;
class AMainCharacter;

UCLASS()
class SIDEPROJECT_API UActionToCameraPoint : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Typed initialization called by the templated factory.
	void Init(APlayerCamera* InPlayerCamera,
		ACameraPoint* InNewCameraPoint,
		UAnimationCurveSubsystem* InAnimSubsystem,
		ACameraManagerMain* InCameraManager,
		const float InPlayRate)
	{
		PlayerCamera = InPlayerCamera;
		NewCameraPoint = InNewCameraPoint;
		AnimSubsystem = InAnimSubsystem;
		CameraManager = InCameraManager;
		PlayRate = InPlayRate;
	}
	
	virtual void OnBegin(bool bFirstTime) override;
	
	virtual void OnUpdate(float DeltaTime) override;
	
	virtual bool IsDone() const override;
	
	//FIELDS:
private:
	UPROPERTY(Transient)
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<ACameraPoint> NewCameraPoint = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UAnimationCurveSubsystem> AnimSubsystem = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<ACameraManagerMain> CameraManager = nullptr;
	
	float PlayRate = 5.0f;
	
	float ElapsedTime = 0.0f;
	
	UPROPERTY(Transient)
	TArray<FTransform> AnimTransforms;
};