#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmHandToLocation.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmHandToLocation : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionArmHandToLocation* Create(UObject* Outer, 
		ABaseInteractableCharacter* InOwnerCharacter, 
		UMoveArm* InMoveArm, 
		const FVector& InToLocation, 
		bool bWantToPickUpItem);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	virtual void OnEnd_Implementation() override;
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	bool bWantToPickUpItem = false;
	
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
	
	UPROPERTY()
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 1.0f;

	UPROPERTY()
	FVector ToLocation = FVector::ZeroVector;
};
