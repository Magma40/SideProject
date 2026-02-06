#include "CameraPointVisualizer.h"

#include "CameraPoint.h"
#include "CameraPointVisualizerComponent.h"
#include "SceneView.h"
#include "PrimitiveDrawInterface.h"
#include "Engine/Selection.h"
#include "Editor.h"

void FCameraPointVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (!Component) return;

	const UCameraPointVisualizerComponent* Marker = Cast<UCameraPointVisualizerComponent>(Component);
	if (!Marker) return;

	const ACameraPoint* CameraPoint = Cast<ACameraPoint>(Marker->GetOwner());
	if (!CameraPoint) return;

	// Optional: only draw if selected
	if (!GEditor->GetSelectedActors()->IsSelected(CameraPoint))
	{
		return;
	}

	const float Slider = CameraPoint->MinMaxSliderValue;
	const FVector Center = CameraPoint->GetActorLocation();
	const FRotator Rot = CameraPoint->GetActorRotation();
	const FVector Right = FRotationMatrix(FRotator(0.f, Rot.Yaw, 0.f)).GetUnitAxis(EAxis::Y);

	PDI->DrawLine(Center - Right * Slider, Center + Right * Slider, FLinearColor::Blue, SDPG_Foreground, 3.f);
}