// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/AnimModelActionStack/ArmActionStack.h"

UArmActionStack::UArmActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UArmActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
}