// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CompanionPawn.h"

ACompanionPawn::ACompanionPawn() { PrimaryActorTick.bCanEverTick = true; }

void ACompanionPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACompanionPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
