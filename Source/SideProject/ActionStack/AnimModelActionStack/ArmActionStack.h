// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ArmActionStack.generated.h"

class UMoveArm;
class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UArmActionStack : public UActionStack
{
	GENERATED_BODY()
	
protected:
	UArmActionStack();
	virtual void BeginPlay() override;

public:
	void Init(UMoveArm*  InArmsRef, ABaseInteractableCharacter* InOwnerCharacter,  ABaseInteractableCharacter* InOwnerCompanion, USceneComponent* InBackHolster);
	void AddMovingAction();
	void AddJumpingAction();
	void AddHolsterAction();
	void AddWaveAction();
	void AddPointingAction();
	void AddHandHoldingAction(UMoveArm* OtherCompanionArm);
	void AddHandToLocation(const FVector& InToLocation, bool bWantToPickUpItem);

	UMoveArm* GetArm() const { return ArmRef; }

private:
	UMoveArm* ArmRef = nullptr;
	UPROPERTY() USceneComponent* BackHolsterRef = nullptr;
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
	ABaseInteractableCharacter* OwnerCompanion = nullptr;
};
