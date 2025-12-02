// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/Characters/ActionCharacter.h"
#include "CompanionCharacter.generated.h"

UCLASS()
class SIDEPROJECT_API UCompanionCharacter : public UActionCharacter
{
	GENERATED_BODY()

public:
	void Initialize(APawn* InActorOwner);
	APawn* GetOwnerCharacter() const { return ActorOwner; }
	void OnBegin_Implementation(bool bFirstTime) override;

	//Actions
	void OnAddActionMoveTo();
	void OnAddActionPickUp();

private:
	UPROPERTY() APawn* ActorOwner = nullptr;
};

