#pragma once

#include "CoreMinimal.h"
#include "PreSwitchCameraPointActor.generated.h"

class APlayerCamera;
class UBoxComponent;
class ACameraPoint;
class UCameraActionStack;

UCLASS()
class SIDEPROJECT_API APreSwitchCameraPointActor : public AActor
{
	GENERATED_BODY()
	
protected:
	APreSwitchCameraPointActor();
	virtual void BeginPlay() override;

	UFUNCTION() void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION() void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	APlayerCamera* PlayerCamera = nullptr;
	UBoxComponent* BoxComponent = nullptr;
	UCameraActionStack* CameraActionStack = nullptr;
	UPROPERTY() bool bHasTriggered = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") bool bDontDoIfJumping = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") ACameraPoint* InsideAreaCameraPoint = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") ACameraPoint* OutsideAreaCameraPoint = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") TArray<ACameraPoint*>  ExceptionalCameraPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") float PlayRate = 1.0f;
};
