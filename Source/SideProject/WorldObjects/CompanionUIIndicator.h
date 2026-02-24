#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Core/PublicVariables.h"
#include "CompanionUIIndicator.generated.h"

class UCanvasPanelSlot;
class APlayerCamera;
class UCompanionWidget;
class UCanvasPanel;
class UImage;
class UTexture2D;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API  UCompanionWidgetIndicator : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UCompanionWidgetIndicator();
	void OnRegister() override;
	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetNewIndicatorImage(ECompanionIndicatorImage NewImageType);
	
private:
	ECompanionIndicatorImage CurrentIndicatorImage = ECompanionIndicatorImage::None;
	UCompanionWidget* CompanionWidget = nullptr;
	UTexture2D* QuestonMarkTexture = nullptr;
	UTexture2D* ExclamationMarkTexture = nullptr;
	UTexture2D* FailedTexture = nullptr;
	APlayerCamera* PlayerCamera = nullptr;
	UCanvasPanelSlot*  PanelSlot = nullptr;
	FVector2D BaseSize = FVector2D(32.f, 32.f);
	float IndicatorShowSpeed = 0.5f;
	bool bDoOnce = true;
};

UCLASS()
class SIDEPROJECT_API UCompanionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UImage* GetImageIndicator() const { return IndicatorImage; }
	UCanvasPanel* GetCanvasPanel() const { return CanvasPanel; }
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY() UCanvasPanel* CanvasPanel;
	UPROPERTY() UImage* IndicatorImage;
};
