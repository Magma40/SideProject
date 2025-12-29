// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionPickUpItem.generated.h"

class UCompanionCharacter;

/**
* Picks up item from ground into hand
 */
UCLASS()
class SIDEPROJECT_API UActionPickUpItem : public UAction
{
	GENERATED_BODY()
	
public:
	UActionPickUpItem(const FObjectInitializer& ObjectInitializer);
	void Init(UCompanionCharacter* InCompanionCharacter);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	UCompanionCharacter* CompanionCharacter = nullptr;
	bool TriedToPickUpItem = false;
};
