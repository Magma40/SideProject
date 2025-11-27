// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionCharacter.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UActionCharacter : public UActionBehaviour
{
	GENERATED_BODY()

public:
	void OnBegin_Implementation(bool bFirstTime) override;
	bool IsDone_Implementation() const override;

	APawn* ActorOwner = nullptr;
};
