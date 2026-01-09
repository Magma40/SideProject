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
	UActionArmPoint(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;
	void OnEnd_Implementation() override;

	void Init(UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter);
	void RenderPointingToIndicator(FVector InHitLocation,  UActorComponent* InPointingAtItemActor);

private:
	UMoveArm* MoveArmRef;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	TArray<FVector> AnimLocations;
	float ElapsedTime = 0.0f;
	float PlayRate = 1.0f;

	TArray<AActor*> PointingIndicators;

public:
	FVector PointingLocation = FVector::ZeroVector;
	UActorComponent* ActorPointedAt = nullptr;
};
