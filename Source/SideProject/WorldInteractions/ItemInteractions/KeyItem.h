// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItemBase.h"
#include "keyItem.generated.h"

UCLASS()
class SIDEPROJECT_API AKeyItem : public AInteractableItemBase
{
	GENERATED_BODY()

protected:
	AKeyItem();
	virtual void Tick(float DeltaTime) override;
};

