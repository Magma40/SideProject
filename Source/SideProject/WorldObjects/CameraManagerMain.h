
#pragma once

#include "CoreMinimal.h"
#include "CameraManagerMain.generated.h"

class UCameraActionStack;
class APlayerCamera;
class ACameraPoint;

UCLASS()
class SIDEPROJECT_API ACameraManagerMain : public AActor
{
	GENERATED_BODY()
	
protected:
	ACameraManagerMain();
	virtual void BeginPlay() override;
	void SetNewCameraPoint(ACameraPoint* NewCameraPoint) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CameraManager") APlayerCamera* PlayerCamera = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CameraManager") UCameraActionStack* CameraActionStack = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CameraManager") ACameraPoint* BaseCameraPoint = nullptr;
};