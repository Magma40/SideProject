#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PlayerCamera.generated.h"

class ACameraPoint;

UCLASS()
class SIDEPROJECT_API APlayerCamera : public ACameraActor
{
	GENERATED_BODY()
	
	APlayerCamera();
	
	//FUNCTIONS:
public:
	//~ Begin Getters & Setters
	UFUNCTION(BlueprintCallable) 
	void SetNewCameraPoint(ACameraPoint* NewCameraPoint);
	
	static APlayerCamera* GetCamera(const UObject* WorldContext = nullptr);
	
	FTransform GetStartCameraTransform() const
	{
		return StartTransform;
	}
	
	ACameraPoint* GetCurrentCameraPoint() const
	{
		return CurrentCameraPoint;
	}
	//~ End Getters & Setters
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<ACameraPoint> CurrentCameraPoint = nullptr;
	
	UPROPERTY()
	FTransform StartTransform = FTransform::Identity;
};
