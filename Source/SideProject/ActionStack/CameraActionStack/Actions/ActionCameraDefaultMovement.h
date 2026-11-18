#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionCameraDefaultMovement.generated.h"

struct FInputActionValue;
class AMainCharacter;
class APlayerCamera;

UCLASS()
class SIDEPROJECT_API UActionCameraDefaultMovement : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionCameraDefaultMovement* Create(UObject* Outer, 
		AMainCharacter* InOwnerCharacter, 
		APlayerCamera* InPlayerCamera);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;

	void LimitLookRangeForCamera(const FInputActionValue& Value);
	
	void LimitMoveRangeForCamera();

	//~ Begin Getters & Setters
	FORCEINLINE void SetRotationLimit(const float InRotationLimit)
	{
		RotationLimit = InRotationLimit;
	}
	//~ End Getters & Setters
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<AMainCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlayerCamera> PlayerCamera = nullptr;
	
	float RotationLimit = 45.0f;
	
	float SliderMoveSpeed = 100.0f;
};
