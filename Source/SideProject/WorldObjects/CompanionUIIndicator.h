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
	
	UCompanionWidgetIndicator();

	//FUNCTIONS:	
public:
	void OnRegister() override;
	
	void BeginPlay() override;
	
	void TickComponent(float DeltaTime, 
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetNewIndicatorImage(ECompanionIndicatorImage NewImageType);
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CompanionWidgetIndicator") 
	float IndicatorShowSpeed = 0.5f;

private:
	ECompanionIndicatorImage CurrentIndicatorImage = ECompanionIndicatorImage::None;
	
	UPROPERTY()
	TObjectPtr<UCompanionWidget> CompanionWidget = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> QuestionMarkTexture = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> ExclamationMarkTexture = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> FailedTexture = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY()
	FVector2D BaseSize = FVector2D(32.f, 32.f);
	
	bool bDoOnce = true;
};

UCLASS()
class SIDEPROJECT_API UCompanionWidget : public UUserWidget
{
	GENERATED_BODY()

	//FUNCTIONS:
public:
	//~ Begin Getters & Setters
	UImage* GetImageIndicator() const
	{
		return IndicatorImage;
	}
	
	UCanvasPanel* GetCanvasPanel() const
	{
		return CanvasPanel;
	}
	
	void SetIndicatorSize(FVector2D NewSize) const;
	//~ End Getters & Setters
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	//FIELDS:
private:
	UPROPERTY() 
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY() 
	TObjectPtr<UImage> IndicatorImage;
};
