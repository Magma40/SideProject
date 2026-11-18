#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmToHolster.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmToHolster : public UAction //Because that's what happens on the ....Polar..... Express!!!!!! //What am I doing bro...
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionArmToHolster* Create(UObject* Outer, 
		UMoveArm* InMoveArm, 
		USceneComponent* InBackHolster,
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
	TObjectPtr<USceneComponent> BackHolsterRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 1.0f;
};
