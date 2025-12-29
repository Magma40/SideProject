// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionLegJumpingMovement.generated.h"

class UMoveLeg;

UCLASS()
class SIDEPROJECT_API UActionLegJumpingMovement : public UAction
{
	GENERATED_BODY()
	
public:
	UActionLegJumpingMovement(const FObjectInitializer& ObjectInitializer);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

	void Init(TArray<UMoveLeg*> InMoveLegs);

private:
	TArray<UMoveLeg*> MoveLegsRef;
};
