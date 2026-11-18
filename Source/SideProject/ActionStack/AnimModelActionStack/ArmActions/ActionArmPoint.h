#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmPoint.generated.h"

class APointingIndicator;
class APlayerCamera;
class UMoveArm;
class ABaseInteractableCharacter;
class AInteractableItemBase;

UCLASS()
class SIDEPROJECT_API UActionArmPoint : public UAction
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	static UActionArmPoint* Create(UObject* Outer, 
		UMoveArm* InMoveArm, 
		ABaseInteractableCharacter* InCharacter, 
		ABaseInteractableCharacter* InOwnerCompanion);
	
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	
	virtual bool IsDone_Implementation() const override;
	
	virtual void OnBegin_Implementation(bool bFirstTime) override;
	
	virtual void OnEnd_Implementation() override;

	void RenderPointingToIndicator();
	
	//~ Begin Getters & Setters
	void GetActorPointingAt();

	FORCEINLINE FVector GetPointingLocation() const
	{
		return PointingLocation;
	} 
	
	FORCEINLINE AActor* GetActorPointedAt() const
	{
		return ActorPointedAt;
	}
	//~ End Getters & Setters
	
	//FIELDS:
private:
	UPROPERTY()
	TObjectPtr<UMoveArm> MoveArmRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABaseInteractableCharacter> OwnerCompanion = nullptr;
	
	UPROPERTY()
	TArray<FTransform> AnimTransforms;
	
	float ElapsedTime = 0.0f;
	
	float PlayRate = 1.0f;

	UPROPERTY() 
	TArray<TObjectPtr<APointingIndicator>> PointingIndicators;
	
	UPROPERTY()
	FVector PointingLocation = FVector::ZeroVector;
	
	UPROPERTY()
	TObjectPtr<AActor> ActorPointedAt = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlayerCamera> TheMainCameraInScene = nullptr;
};
