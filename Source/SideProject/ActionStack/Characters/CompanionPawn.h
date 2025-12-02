// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CompanionCharacter.h"  
#include "Player/BaseInteractableCharacter.h"
#include "CompanionPawn.generated.h"

UCLASS()
class SIDEPROJECT_API ACompanionPawn : public ABaseInteractableCharacter
{
	GENERATED_BODY()

public:
	ACompanionPawn();
	UFUNCTION(BlueprintCallable) UCompanionCharacter* GetCompanionData() const { return CompanionData; }
	UFUNCTION(BlueprintCallable) void SetMoveToLocation(FVector InLocation){ Location = InLocation; }
	UFUNCTION(BlueprintCallable) FVector GetMoveToLocation(){ return Location; }

	UFUNCTION(BlueprintCallable) void AddNewMoveTo(FVector InLocation);
	UFUNCTION(BlueprintCallable) void AddPickUpItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UCompanionCharacter* CompanionData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FVector Location;
	bool DefaultMovement = true;
};
