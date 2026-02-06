// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldObjects/PlayerCamera.h"

#include "Public/CameraPoint.h"
#include "Kismet/GameplayStatics.h"

APlayerCamera::APlayerCamera()  { PrimaryActorTick.bCanEverTick = true; }

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCamera::SetNewCameraPoint(ACameraPoint* NewCameraPoint)
{
	if(!IsValid(NewCameraPoint)) return;
	const FTransform NewCameraTransform = NewCameraPoint->GetActorTransform();
	StartTransform = NewCameraTransform;
	CurrentCameraPoint = NewCameraPoint;
}

APlayerCamera* APlayerCamera::GetCamera(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	const UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	return Cast<APlayerCamera>(UGameplayStatics::GetActorOfClass(World,APlayerCamera::StaticClass()));
}
