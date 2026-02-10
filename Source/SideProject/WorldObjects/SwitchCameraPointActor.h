#pragma once

#include "CoreMinimal.h"
#include "SwitchCameraPointActor.generated.h"

class UBoxComponent;
class ACameraPoint;
class UCameraActionStack;

UCLASS()
class SIDEPROJECT_API ASwitchCameraPointActor : public AActor
{
	GENERATED_BODY()
	
protected:
	ASwitchCameraPointActor();
	virtual void BeginPlay() override;

	UFUNCTION() void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION() void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UBoxComponent* BoxComponent = nullptr;
	UCameraActionStack* CameraActionStack = nullptr;
	UPROPERTY() bool bHasTriggered = false;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SwitchCameraPointActor") ACameraPoint* CameraPoint_L = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SwitchCameraPointActor") ACameraPoint* CameraPoint_R = nullptr;
};