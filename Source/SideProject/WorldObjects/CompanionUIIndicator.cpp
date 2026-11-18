#include "CompanionUIIndicator.h"
#include "PlayerCamera.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CompanionPawn.h"

UCompanionWidgetIndicator::UCompanionWidgetIndicator()
{
	SetWidgetClass(UCompanionWidget::StaticClass());

	static ConstructorHelpers::FObjectFinder<UTexture2D> QuestionMarkTextureFinder(TEXT("/Game/Characters/CharacterBlueprints/AI/QuestionMarkOutlined.QuestionMarkOutlined"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ExclamationMarkTextureFinder(TEXT("/Game/Characters/CharacterBlueprints/AI/ExclamationMarkOutlined.ExclamationMarkOutlined"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> FailedMarkTextureFinder(TEXT("/Engine/EditorMaterials/Cross.Cross"));

	if (QuestionMarkTextureFinder.Succeeded() && ExclamationMarkTextureFinder.Succeeded() && FailedMarkTextureFinder.Succeeded())
	{
		QuestionMarkTexture = QuestionMarkTextureFinder.Object;
		ExclamationMarkTexture = ExclamationMarkTextureFinder.Object;
		FailedTexture = FailedMarkTextureFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s:UCompanionWidgetIndicator - Textures not found or wrong type"), *StaticClass()->GetName());
		return;
	}
}

void UCompanionWidgetIndicator::OnRegister()
{
	Super::OnRegister();
	
	if (ACompanionPawn* OwnerCompanion = Cast<ACompanionPawn>(GetOwner()))
	{
		OwnerCompanion->SetCompanionWidgetIndicator(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is not ACompanionPawn"));
	}
}

void UCompanionWidgetIndicator::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* FindActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass());
	PlayerCamera = Cast<APlayerCamera>(FindActor);
	
	InitWidget(); 
	CompanionWidget = Cast<UCompanionWidget>(GetWidget());
}

void UCompanionWidgetIndicator::TickComponent(float DeltaTime, 
	enum ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(PlayerCamera))
	{
		return;
	}
	
	this->SetWorldRotation((PlayerCamera->GetActorLocation() - GetComponentLocation()).Rotation());

	if (bDoOnce && 
		CompanionWidget && 
		CompanionWidget->GetImageIndicator()) //this is stupid, widget won't get ImageIndicator in BeginPlay?
	{
		CompanionWidget->GetImageIndicator()->SetBrushFromTexture(FailedTexture, true);
		CompanionWidget->GetImageIndicator()->SetVisibility(ESlateVisibility::Hidden);
		
		bDoOnce = false;
	}

	if (IsValid(CompanionWidget) && 
		IsValid(CompanionWidget->GetImageIndicator()) && 
		CurrentIndicatorImage != ECompanionIndicatorImage::None)
	{
		const UImage* Image = CompanionWidget->GetImageIndicator();
		
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image->Slot))
		{
			const FVector2D CurrentSize = CanvasSlot->GetSize();
			
			if (CurrentSize.X < BaseSize.X)
			{
				const FVector2D NewSize = CurrentSize + IndicatorShowSpeed;
				CanvasSlot->SetSize(NewSize);
			}
		}
	}
}

void UCompanionWidgetIndicator::SetNewIndicatorImage(const ECompanionIndicatorImage NewImageType)
{
	if (CurrentIndicatorImage != NewImageType)
	{
		CompanionWidget->SetIndicatorSize(FVector2D::Zero());
	}
	
	UImage* IndicatorImage = CompanionWidget->GetImageIndicator();
	if (!IsValid(IndicatorImage))
	{
		return;
	}
	
	UTexture2D* IndicatorTexture = nullptr;
	IndicatorImage->SetVisibility(ESlateVisibility::Visible);

	switch (NewImageType)
	{
		case ECompanionIndicatorImage::QuestionMark : IndicatorTexture = QuestionMarkTexture; break;
		case ECompanionIndicatorImage::ExclamationMark : IndicatorTexture = ExclamationMarkTexture; break;
		case ECompanionIndicatorImage::Failed : IndicatorTexture = FailedTexture; break;
		case ECompanionIndicatorImage::None:
		{
			IndicatorTexture = nullptr;
			IndicatorImage->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	}
	
	CurrentIndicatorImage = NewImageType;
	IndicatorImage->SetBrushFromTexture(IndicatorTexture, true);
}

TSharedRef<SWidget> UCompanionWidget::RebuildWidget()
{
	Super::RebuildWidget();

	CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasPanel"));
	WidgetTree->RootWidget = CanvasPanel;

	IndicatorImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IndicatorImage"));

	UCanvasPanelSlot* CanvasSlot = CanvasPanel->AddChildToCanvas(IndicatorImage);

	CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CanvasSlot->SetPosition(FVector2D(0.f, 0.f));
	CanvasSlot->SetSize(FVector2D(32.f, 32.f));

	return Super::RebuildWidget();
}

void UCompanionWidget::SetIndicatorSize(const FVector2D NewSize) const
{
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IndicatorImage->Slot))
	{
		CanvasSlot->SetSize(NewSize);
	}
}
