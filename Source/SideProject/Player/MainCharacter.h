// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MainCharacter.generated.h"

class UInputAction;
struct FInputActionValue;
class AMovingModel;

/**
 Class for the player
 */
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
UCLASS(abstract)
class AMainCharacter : public ABaseInteractableCharacter
{
	GENERATED_BODY()
	
protected: //Functions
	AMainCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Movement
	void Move(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoMove(float Right, float Forward);
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpStart() { Jump(); }
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpEnd() { StopJumping(); }
	UInputAction* GetLookAction() const { return LookAction; }
	UInputAction* GetMoveAction() const { return MoveAction; }
	UFUNCTION(BlueprintCallable) virtual void RespawnCharacter() override;

protected: //Variables
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float JumpSpeedForceChange = 100.0f;
	float DefaultMaxSprintSpeed = 1.0f;
};

