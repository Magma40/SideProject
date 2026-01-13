// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CompanionPawn.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "Tools/GenericCalls.h"

ACompanionPawn::ACompanionPawn() { PrimaryActorTick.bCanEverTick = true; }

void ACompanionPawn::BeginPlay()
{
	Super::BeginPlay();
	// Get CompanionActionStack
	CompanionActionStack = Cast<UCompanionActionStack>(GenericCalls::FindChildByExactName(this, TEXT("CompanionActionStack")));
	if (!CompanionActionStack)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - CompanionActionStack not found or wrong type"), *StaticClass()->GetName());
		return;
	}
}

void ACompanionPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
