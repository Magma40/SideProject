// Fill out your copyright notice in the Description page of Project Settings.
#include "CameraActionStack.h"
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "ActionStack/CameraActionStack/Actions/ActionToCameraPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainCharacter.h"
#include "WorldObjects/PlayerCamera.h"

UCameraActionStack::UCameraActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UCameraActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
	Init();
	PushDefaultActionMovement();
}

void UCameraActionStack::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCameraActionStack::Init()
{
	OwnerCharacter = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));
    PlayerCamera = APlayerCamera::GetCamera(this);
}

void UCameraActionStack::PushDefaultActionMovement()
{
	UActionCameraDefaultMovement* DefaultMovement = UActionCameraDefaultMovement::Create(this, OwnerCharacter, PlayerCamera);
	PushAction(DefaultMovement);
}

void UCameraActionStack::PushToCameraPointAction(ACameraPoint* InNewCameraPoint)
{
	UActionToCameraPoint* ToCameraPoint = UActionToCameraPoint::Create(this, PlayerCamera, OwnerCharacter, InNewCameraPoint);
	PushAction(ToCameraPoint);
}
