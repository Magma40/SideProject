// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CompanionActionStack.generated.h"

class ABaseInteractableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UCompanionActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	UCompanionActionStack();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Init();

public:
	//Actions
	void OnAddActionMoveTo(const FVector& InMoveToLocation, bool InNeedIndicator);
	void OnAddActionPickUp();
	void OnAddActionContinuousMove(ABaseInteractableCharacter* InPlayer);

private:
	ABaseInteractableCharacter* OwnerCharacter = nullptr;
};
