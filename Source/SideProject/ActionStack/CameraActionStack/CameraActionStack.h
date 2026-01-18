// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "CameraActionStack.generated.h"

class APlayerCamera;
class AMainCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UCameraActionStack : public UActionStack
{
	GENERATED_BODY()

protected:
	UCameraActionStack();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Init();
	
	AMainCharacter* OwnerCharacter = nullptr;
	APlayerCamera* PlayerCamera = nullptr;
public:
 	void PushDefaultActionMovement();
	UFUNCTION(BlueprintCallable) void PushToCameraPointAction();
};
