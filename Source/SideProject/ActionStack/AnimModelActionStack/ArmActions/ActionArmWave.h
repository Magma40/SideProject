#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmWave.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmWave : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionArmWave* Create(UObject* Outer, 
		UMoveArm* InMoveArm, 
		ABaseInteractableCharacter* InCharacter);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	virtual void OnEnd_Implementation() override;

	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 1.0f;
};