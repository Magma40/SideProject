// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class UInputAction;
struct FInputActionValue;

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
	void ContinuousFalling(float Right, float Forward);

public:
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoMove(float Right, float Forward);
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpStart() { Jump(); }
	UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpEnd() { StopJumping(); }

protected: //Variables
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* MouseLookAction;

	UPROPERTY() ACameraActor* TheMainCameraInScene = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") bool Sprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float MaxSprintSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float WalkSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") bool EnabledInput = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float JumpSpeedForceChange = 100.0f;
	
	float LastOldForwardValue = 0.0f;
	float LastOldRightValue = 0.0f;
	float DefaultMaxSprintSpeed = 1.0f;

public:
	UPROPERTY() UActionStack* ActionStack;
};

