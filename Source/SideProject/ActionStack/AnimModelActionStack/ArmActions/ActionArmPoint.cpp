// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmPoint.h"

#include "Camera/CameraActor.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
UActionArmPoint::UActionArmPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void UActionArmPoint::Init(UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter)
{
	MoveArmRef = InMoveArm;
	OwnerCharacter = InCharacter;
}

void UActionArmPoint::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimLocations.Insert(HandLocationPos, 0);
	
	FHitResult Hit;
	FVector Start = OwnerCharacter->TheMainCameraInScene->GetActorLocation();
	FVector End = Start + (OwnerCharacter->TheMainCameraInScene->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		PointingLocation = Hit.ImpactPoint;
		FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
		AnimLocations.Insert(PointLocation, 1);
	}
}

void UActionArmPoint::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	while (PointingIndicators.Num() > 0)
	{
		AActor* Extra = PointingIndicators.Pop();
		if (Extra) Extra->Destroy();
	}
}

void UActionArmPoint::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	if (AnimLocations.Num() > 1)
	{
		UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
		if (!AnimSubsystem)
		{
			UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
			return;
		}
		const TTuple<TTuple<TArray<FVector>, float>,FVector> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimLocations, ElapsedTime, PlayRate);
		MoveArmRef->TargetPose.Position = NewValues.Value;
		AnimLocations = NewValues.Key.Key;
		ElapsedTime = NewValues.Key.Value;
		return;
	}

	FHitResult Hit;
	FVector Start = OwnerCharacter->TheMainCameraInScene->GetActorLocation();
	FVector End = Start + (OwnerCharacter->TheMainCameraInScene->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		RenderPointingToIndicator(PointingLocation);
		if (Hit.GetComponent()->GetOwner()->ActorHasTag("HandHeldItem"))
		{
			ActorPointedAt = Hit.GetComponent();
		}
		else ActorPointedAt = nullptr;
		
		PointingLocation = Hit.ImpactPoint;
		const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
		FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
		MoveArmRef->TargetPose.Position = PointLocation;
		MoveArmRef->TargetPose.Rotation = FQuat::Identity;
		DrawDebugLine(GetWorld(),Start + (OwnerCharacter->TheMainCameraInScene->GetActorForwardVector() * 100.f),Hit.Location,FColor(255, 0, 0),false, -1, 0,1);
	}
	else
	{
		ActorPointedAt = nullptr;
		const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
		FVector PointLocation = Xform.InverseTransformPosition(End);
		MoveArmRef->TargetPose.Position = PointLocation;
		MoveArmRef->TargetPose.Rotation = FQuat::Identity;
	}
}

bool UActionArmPoint::IsDone_Implementation() const
{
	return false;
}

void UActionArmPoint::RenderPointingToIndicator(FVector InHitLocation)
{
	if (!MoveArmRef || MoveArmRef->ArmParts.Num() < 3) return;

	const float EachSpawnDistance = 25.0f;
	FVector Start = MoveArmRef->ArmParts[2]->GetComponentLocation();
	FVector Direction = (InHitLocation - Start).GetSafeNormal();
	float TotalDistance = FVector::Distance(InHitLocation, Start);
	int32 RequiredCount = FMath::FloorToInt(TotalDistance / EachSpawnDistance);

	// Spawn new indicators if we need more
	while (PointingIndicators.Num() < RequiredCount)
	{
		AActor* NewIndicator = GetWorld()->SpawnActor<AActor>();
		USceneComponent* Root = NewObject<USceneComponent>(NewIndicator);
		Root->RegisterComponent();
		NewIndicator->SetRootComponent(Root);
		
		// Inside the spawned actor class (or dynamically added component)
		UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(NewIndicator);
		Mesh->SetupAttachment(Root);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetHiddenInGame(false);

		static UStaticMesh* SphereMesh = nullptr;
		if (!SphereMesh)
		{
			SphereMesh = LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		}
		//UMaterialInterface* _ghostMaterial = nullptr;
		//FString ghostMaterialName = "/Game/Materials/ActionStackMat/Mat_OutlineSeeThrough.Mat_OutlineSeeThrough";
		//static ConstructorHelpers::FObjectFinder<UMaterialInterface> ghostMaterialAsset(*ghostMaterialName);
		//_ghostMaterial = ghostMaterialAsset.Object;
		
		//SphereMesh->SetMaterial(0,   _ghostMaterial);
		//Mesh->SetMaterial(0,  _ghostMaterial);
		
		Mesh->SetStaticMesh(SphereMesh);
		Mesh->SetWorldScale3D(FVector(0.1f));
		Mesh->RegisterComponent();
		
		PointingIndicators.Add(NewIndicator);
	}

	// Destroy extra indicators if we need fewer
	while (PointingIndicators.Num() > RequiredCount)
	{
		AActor* Extra = PointingIndicators.Pop();
		if (Extra) Extra->Destroy();
	}

	// Move indicators and draw debug spheres for visualization
	for (int32 i = 0; i < PointingIndicators.Num(); i++)
	{
		FVector NewLocation = Start + Direction * EachSpawnDistance * (i + 1);
		if (PointingIndicators[i])
		{
			PointingIndicators[i]->SetActorLocation(NewLocation);
		}
	}
}
