// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionPawn.h"
#include "ActionStack/ActionStack.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainCharacter.h"
#include "ProcedualAnimation/MovingModel.h"


ACompanionPawn::ACompanionPawn() { PrimaryActorTick.bCanEverTick = true; }

void ACompanionPawn::BeginPlay()
{
	Super::BeginPlay();
	CompanionData = NewObject<UCompanionCharacter>(this);
	CompanionData->Initialize(this);
}

void ACompanionPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACompanionPawn::AddNewMoveTo(FVector InLocation)
{
	SetMoveToLocation(InLocation);
	CompanionData->OnAddActionMoveTo();
}

void ACompanionPawn::AddNewActionContinuousMoveTo(ABaseInteractableCharacter* InPlayer)
{
	CompanionData->OnAddActionContinuousMove(InPlayer);
}

void ACompanionPawn::AddPickUpItem()
{
	CompanionData->OnAddActionPickUp();
}
