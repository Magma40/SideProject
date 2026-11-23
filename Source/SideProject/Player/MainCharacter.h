#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MainCharacter.generated.h"

class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 Class for the player
 */

UCLASS(abstract)
class AMainCharacter : public ABaseInteractableCharacter
{
	GENERATED_BODY()
	
	AMainCharacter();
	
	//FUNCTIONS:
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintCallable, Category="Input") 
	virtual void DoMove(float Right, float Forward);
	
	UFUNCTION(BlueprintCallable, Category="Input") 
	virtual void DoJumpStart()
	{
		Jump();
	}
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd()
	{
		StopJumping();
	}
	
	UFUNCTION(BlueprintCallable) 
	virtual void RespawnCharacter() override;
	
	//~ Begin Getters & Setters
	FORCEINLINE UInputAction* GetLookAction() const
	{
		return LookAction;
	}
	
	FORCEINLINE UInputAction* GetMoveAction() const
	{
		return MoveAction;
	}
	//~ End Getters & Setters

	//FIELDS:
protected:
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement") float JumpSpeedForceChange = 100.0f;
	float DefaultMaxSprintSpeed = 1.0f;
};

