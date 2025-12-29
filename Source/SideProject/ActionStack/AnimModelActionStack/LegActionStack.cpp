// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/AnimModelActionStack/LegActionStack.h"

ULegActionStack::ULegActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void ULegActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}