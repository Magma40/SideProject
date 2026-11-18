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

	APreSwitchCameraPointActor();

	//FUNCTIONS:
protected:
	virtual void BeginPlay() override;

	UFUNCTION() 
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult & SweepResult);
	
	UFUNCTION() 
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") 
	bool bDontDoIfJumping = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") 
	TObjectPtr<ACameraPoint> InsideAreaCameraPoint = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") 
	TObjectPtr<ACameraPoint> OutsideAreaCameraPoint = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor") 
	TArray<TObjectPtr<ACameraPoint>>  ExceptionalCameraPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "PreSwitchCameraPointActor")
	float PlayRate = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY()
	TObjectPtr<UBoxComponent> BoxComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCameraActionStack> CameraActionStack = nullptr;
	
	UPROPERTY() 
	bool bHasTriggered = false;
};
