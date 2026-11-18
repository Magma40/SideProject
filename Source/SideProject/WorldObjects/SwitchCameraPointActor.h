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
	
	ASwitchCameraPointActor();
	
	//FUNCTIONS:
protected:
	virtual void BeginPlay() override;

	UFUNCTION() 
	void PositiveOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult & SweepResult);
	
	UFUNCTION() 
	void PositiveOverlapEnd(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

	UFUNCTION() 
	void NegativeOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult & SweepResult);
	
	UFUNCTION() 
	void NegativeOverlapEnd(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);
	
	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SwitchCameraPointActor") 
	bool bDontDoIfJumping = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SwitchCameraPointActor") 
	TObjectPtr<ACameraPoint> CameraPoint_L = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SwitchCameraPointActor") 
	TObjectPtr<ACameraPoint> CameraPoint_R = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "SwitchCameraPointActor") 
	float PlayRate = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<UBoxComponent> PositiveBoxComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UBoxComponent> NegativeBoxComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCameraActionStack> CameraActionStack = nullptr;
	
	UPROPERTY() 
	bool bHasTriggered = false;
};