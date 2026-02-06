#include "SideProjectEditor.h"
#include "UnrealEd.h"
#include "CameraPointVisualizer.h"
#include "CameraPointVisualizerComponent.h"
#include "Components/SceneComponent.h"
#include "Editor.h"

IMPLEMENT_MODULE(FSideProjectEditorModule, SideProjectEditor)

void FSideProjectEditorModule::StartupModule()
{
	if (GIsEditor)
	{
		FEditorDelegates::OnMapOpened.AddRaw(this, &FSideProjectEditorModule::RegisterVisualizers);

	}
}

void FSideProjectEditorModule::ShutdownModule()
{
	if (GUnrealEd && CameraPointVisualizer.IsValid())
	{
		GUnrealEd->UnregisterComponentVisualizer(
			UCameraPointVisualizerComponent::StaticClass()->GetFName()
		);
	}

	CameraPointVisualizer.Reset();
}

void FSideProjectEditorModule::RegisterVisualizers(const FString& MapName, bool /*bAsTemplate*/)
{
	if (!GUnrealEd) return;

	CameraPointVisualizer = MakeShareable(new FCameraPointVisualizer());

	GUnrealEd->RegisterComponentVisualizer(
		UCameraPointVisualizerComponent::StaticClass()->GetFName(),
		CameraPointVisualizer
	);

	CameraPointVisualizer->OnRegister();
}