#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CameraActionStack.generated.h"

class APlayerCamera;
class AMainCharacter;
class ACameraPoint;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UCameraActionStack : public UActionStack
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	void PushDefaultActionMovement();
	
	UFUNCTION(BlueprintCallable)
	void PushToCameraPointAction(ACameraPoint* InNewCameraPoint, float InPlayRate);

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void Init();
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<AMainCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
};
