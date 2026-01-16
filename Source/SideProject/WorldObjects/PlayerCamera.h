// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PlayerCamera.generated.h"

UCLASS()
class SIDEPROJECT_API APlayerCamera : public ACameraActor
{
	GENERATED_BODY()
	
protected:
	APlayerCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
 	UFUNCTION(BlueprintCallable) void SetNewCameraPoint(AActor* NewCameraPoint);
	static APlayerCamera* GetCamera(const UObject* WorldContext = nullptr);
	FTransform GetStartCameraTransform() const { return StartTransform; }
	AActor* GetCurrentCameraPoint() const { return CurrentCameraPoint; }

private:
	AActor* CurrentCameraPoint = nullptr;
	FTransform StartTransform = FTransform::Identity;
};
