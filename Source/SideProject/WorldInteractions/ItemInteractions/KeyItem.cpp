// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ItemInteractions/KeyItem.h"

AKeyItem::AKeyItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}


void AKeyItem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}
