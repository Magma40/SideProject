#pragma once

#include "CoreMinimal.h"
#include "OutOfBoundsCollider.generated.h"

class UBoxComponent;

UCLASS()
class SIDEPROJECT_API AOutOfBoundsCollider : public AActor
{
	GENERATED_BODY()
	
protected:
	AOutOfBoundsCollider();
	virtual void BeginPlay() override;

	UFUNCTION() void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UBoxComponent* BoxComponent = nullptr;
};