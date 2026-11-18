#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionToCameraPoint.generated.h"

class APlayerCamera;
class ACameraPoint;
class AMainCharacter;

UCLASS()
class SIDEPROJECT_API UActionToCameraPoint : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionToCameraPoint* Create(UObject* Outer, 
		APlayerCamera* InPlayerCamera, 
		AMainCharacter* InOwnerCharacter, 
		ACameraPoint* InNewCameraPoint, 
		float InPlayRate);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<AMainCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ACameraPoint> NewCameraPoint = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	UPROPERTY()
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 5.0f;
};
