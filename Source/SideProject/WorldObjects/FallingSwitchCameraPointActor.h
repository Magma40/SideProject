#pragma once

#include "CoreMinimal.h"
#include "FallingSwitchCameraPointActor.generated.h"

class UBoxComponent;
class ACameraPoint;
class UCameraActionStack;

UCLASS()
class SIDEPROJECT_API AFallingSwitchCameraPointActor : public AActor
{
	GENERATED_BODY()
	
	AFallingSwitchCameraPointActor();
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "FallingSwitchCameraPointActor") 
	TObjectPtr<ACameraPoint> FallBackCameraPoint = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "FallingSwitchCameraPointActor")
	float PlayRate = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<UBoxComponent> BoxComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCameraActionStack> CameraActionStack = nullptr;
	
	UPROPERTY() 
	bool bHasTriggered = false;
};
