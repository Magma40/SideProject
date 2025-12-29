// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/ActionStackManager.h"
#include "ActionStack.h"

AActionStackManager::AActionStackManager()
{
	PrimaryActorTick.bCanEverTick = true;
	ActionStack = CreateDefaultSubobject<UActionStack>(TEXT("ActionStack"));
}

void AActionStackManager::BeginPlay()
{
	Super::BeginPlay();
}


