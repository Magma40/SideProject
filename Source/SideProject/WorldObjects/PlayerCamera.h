// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PlayerCamera.generated.h"

class ACameraPoint;

UCLASS()
class SIDEPROJECT_API APlayerCamera : public ACameraActor
{
	GENERATED_BODY()
	
protected:
	APlayerCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
 	UFUNCTION(BlueprintCallable) void SetNewCameraPoint(ACameraPoint* NewCameraPoint);
	static APlayerCamera* GetCamera(const UObject* WorldContext = nullptr);
	FTransform GetStartCameraTransform() const { return StartTransform; }
	ACameraPoint* GetCurrentCameraPoint() const { return CurrentCameraPoint; }

private:
	ACameraPoint* CurrentCameraPoint = nullptr;
	FTransform StartTransform = FTransform::Identity;
};
