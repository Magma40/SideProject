#include "CameraPoint.h"

#include "CameraPointVisualizerComponent.h"
#include "Components/SceneComponent.h"

ACameraPoint::ACameraPoint()
{
	VisualizerRoot = CreateDefaultSubobject<UCameraPointVisualizerComponent>(TEXT("VisualizerRoot"));
	RootComponent = VisualizerRoot;
}