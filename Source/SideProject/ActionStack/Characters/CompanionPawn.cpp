// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionPawn.h"
#include "ActionStack/ActionStack.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainCharacter.h"

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
	// if (DefaultMovement)
	// {
	// 	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));
	// 	if (!IsValid(MainCharacter)) return;
	//
	// 	SetMoveToLocation(MainCharacter->GetActorLocation());
	// 	CompanionData->OnBegin_Implementation(true);	
	// }
}

void ACompanionPawn::AddNewMoveTo(FVector InLocation)
{
	DefaultMovement = false;
	SetMoveToLocation(InLocation);
	CompanionData->OnBegin_Implementation(true);
}
