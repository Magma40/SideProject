#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FComponentVisualizer;

class FSideProjectEditorModule : public IModuleInterface
{
public:

    void RegisterVisualizers(const FString& MapName, bool /*bAsTemplate*/);
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedPtr<FComponentVisualizer> CameraPointVisualizer;
};
