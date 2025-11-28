// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ActionStack/ActionStack.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SIDEPROJECT_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainGameMode();

	// The single ActionStack instance for the game
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UActionStack* ActionStack;
};
