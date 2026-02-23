#pragma once


#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "CompanionUIIndicator.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCompanionWidgetIndicator : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)  UUserWidget* CurrentWidget = nullptr;
};
