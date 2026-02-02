// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItemBase.h"
#include "KatanaItem.generated.h"

UCLASS()
class SIDEPROJECT_API AKatanaItem : public AInteractableItemBase
{
	GENERATED_BODY()

protected:
	AKatanaItem();
	virtual void Tick(float DeltaTime) override;
};

