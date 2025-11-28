// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionPawn.h"
#include "ActionStack/ActionStack.h" 

ACompanionPawn::ACompanionPawn() { PrimaryActorTick.bCanEverTick = true; }

void ACompanionPawn::BeginPlay()
{
	Super::BeginPlay();
	CompanionData = NewObject<UCompanionCharacter>(this);
	CompanionData->Initialize(this);
	CompanionData->OnBegin(true);
}

