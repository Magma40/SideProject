
#pragma once

#include "CoreMinimal.h"
#include "CameraManagerMain.generated.h"

class ABaseInteractableCharacter;
class UCameraActionStack;
class APlayerCamera;
class ACameraPoint;

UCLASS()
class SIDEPROJECT_API ACameraManagerMain : public AActor
{
	GENERATED_BODY()
	
	ACameraManagerMain();
	
	//FUNCTIONS:
protected:
	virtual void BeginPlay() override;

	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CameraManager") 
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CameraManager")
	TObjectPtr<UCameraActionStack> CameraActionStack = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CameraManager") 
	TObjectPtr<ACameraPoint> BaseCameraPoint = nullptr;
};