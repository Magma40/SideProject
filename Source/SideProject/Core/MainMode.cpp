// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameMode.h"

AMainGameMode::AMainGameMode()
{
	// Create the ActionStack component and attach to GameMode
	ActionStack = CreateDefaultSubobject<UActionStack>(TEXT("ActionStack"));
}