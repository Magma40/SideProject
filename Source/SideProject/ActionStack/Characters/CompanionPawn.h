// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CompanionCharacter.h"  
#include "CompanionPawn.generated.h"

UCLASS()
class SIDEPROJECT_API ACompanionPawn : public APawn
{
	GENERATED_BODY()

public:
	ACompanionPawn();
	UFUNCTION(BlueprintCallable) UCompanionCharacter* GetCompanionData() const { return CompanionData; }

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UCompanionCharacter* CompanionData;
};
