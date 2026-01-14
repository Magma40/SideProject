// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionArmPoint.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionArmPoint : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionArmPoint* Create(UObject* Outer, UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
	void OnEnd_Implementation() override;

	void RenderPointingToIndicator(const FVector& InHitLocation, const UActorComponent* InPointingAtItemActor);

	FVector GetPointingLocation() const { return PointingLocation; } 
	UActorComponent* GetActorPointedAt() const { return ActorPointedAt; }
	
private:
	UMoveArm* MoveArmRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	TArray<FTransform> AnimTransforms;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;

	TArray<AActor*> PointingIndicators;
	FVector PointingLocation = FVector::ZeroVector;
	UActorComponent* ActorPointedAt = nullptr;
};
