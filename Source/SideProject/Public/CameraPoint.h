#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraPoint.generated.h"

class UCameraPointVisualizerComponent;
UCLASS()
class SIDEPROJECT_API ACameraPoint : public AActor
{
	GENERATED_BODY()

public:
	ACameraPoint();
	UPROPERTY(VisibleAnywhere) UCameraPointVisualizerComponent * VisualizerRoot = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera") float MinMaxSliderValue = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Camera") FTransform PlayerRoomSpawnPoint  = FTransform::Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Camera") FTransform CompanionRoomSpawnPoint  = FTransform::Identity;
};
