#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Limb.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API ULimb : public UStaticMeshComponent
{
	GENERATED_BODY()
	
	ULimb();
	
	//FUNCTIONS:
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	//FIELDS:
public:
	UPROPERTY()
	TObjectPtr<USceneComponent> AttachedParent = nullptr;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> AttachedChild = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Limb")
	FString AttachedParentName;
	
	UPROPERTY(EditAnywhere, Category = "Limb")
	FString AttachedChildName; 
};
