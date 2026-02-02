// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ItemInteractions/KatanaItem.h"

AKatanaItem::AKatanaItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AKatanaItem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}