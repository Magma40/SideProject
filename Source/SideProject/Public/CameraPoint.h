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
	UPROPERTY(VisibleAnywhere) UCameraPointVisualizerComponent * VisualizerRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera") ACameraPoint* CameraPointLeft = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera") ACameraPoint* CameraPointRight = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera") float MinMaxSliderValue = 1000.0f;
};
