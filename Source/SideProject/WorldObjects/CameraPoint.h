#pragma once

#include "CoreMinimal.h"
#include "CameraPoint.generated.h"

UCLASS()
class SIDEPROJECT_API ACameraPoint : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera") float MinMaxSliderValue = 1000.0f;
};
