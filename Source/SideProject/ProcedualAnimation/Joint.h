#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Joint.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UJoint : public UStaticMeshComponent
{
	GENERATED_BODY()
	
	UJoint();
	
	//FUNCTIONS:
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	//FIELDS:
public:
	UPROPERTY()
	TObjectPtr<USceneComponent> JointReference = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Joint")
	FString JointName;
};
