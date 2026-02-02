// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"
#include "WorldObjects/PlayerCamera.h"

UActionArmPoint* UActionArmPoint::Create(UObject* Outer, UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter)
{
	UActionArmPoint* Action = NewObject<UActionArmPoint>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->OwnerCharacter = InCharacter;
	Action->TheMainCameraInScene = APlayerCamera::GetCamera(InCharacter->GetWorld());
	return Action;
}

void UActionArmPoint::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmPoint::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	RenderPointingToIndicator(PointingLocation,  nullptr);
	while (PointingIndicators.Num() > 0)
	{
		AActor* Extra = PointingIndicators.Pop();
		if (Extra) Extra->Destroy();
	}
}

void UActionArmPoint::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);

	FHitResult Hit;
	const FVector Start = TheMainCameraInScene->GetActorLocation();
	const FVector End = Start + (TheMainCameraInScene->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "InvisibleWall", ActorsToIgnore);
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(TheMainCameraInScene);
	Params.AddIgnoredActors(ActorsToIgnore);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableItemInterface::StaticClass()))
		{
			ActorPointedAt = Cast<AInteractableItemBase>(HitActor);
			RenderPointingToIndicator(Hit.ImpactPoint, ActorPointedAt);
		}
		else
		{
			RenderPointingToIndicator(Hit.ImpactPoint, nullptr);
			ActorPointedAt = nullptr;
		}
	}

	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FQuat RelativeQuat;

	FVector LocationA = MoveArmRef->ArmParts[1]->GetComponentLocation();
	FVector LocationB = MoveArmRef->ArmParts[2]->GetComponentLocation();
	FVector Direction = LocationB - LocationA;
	Direction.Normalize();
	FVector NewZAxis = Direction;
	FVector WorldUpVector = FVector::UpVector; 
	FVector NewXAxis = FVector::CrossProduct(WorldUpVector, NewZAxis).GetSafeNormal();
	if (NewXAxis.IsNearlyZero())
	{
		FVector WorldRightVector = FVector::RightVector;
		NewXAxis = FVector::CrossProduct(WorldRightVector, NewZAxis).GetSafeNormal();
	}

	FVector NewYAxis = FVector::CrossProduct(NewZAxis, NewXAxis).GetSafeNormal();
	FMatrix Matrix( FPlane(NewXAxis, 0.f), FPlane(NewYAxis, 0.f), FPlane(NewZAxis, 0.f), FPlane(0.f, 0.f, 0.f, 1.f));
	FRotator NewRotation = Matrix.Rotator();
	RelativeQuat = Xform.InverseTransformRotation(NewRotation.Quaternion());
	
	AnimTransforms.Empty();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(RelativeQuat, HandLocationPos));

	FVector PointLocation = Xform.InverseTransformPosition(PointingLocation);
	AnimTransforms.Add(FTransform(RelativeQuat, PointLocation));

	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
}

bool UActionArmPoint::IsDone_Implementation() const
{
	return false;
}

void UActionArmPoint::RenderPointingToIndicator(const FVector& InHitLocation, AInteractableItemBase* InPointingAtItemActor)
{
	if (!MoveArmRef || MoveArmRef->ArmParts.Num() < 3) return;
	PointingLocation = InHitLocation;

	constexpr float EachSpawnDistance = 25.0f;
	const FVector Start = MoveArmRef->ArmParts[2]->GetComponentLocation();

	const float TotalDistance = FVector::Distance(InHitLocation, Start);
	const int32 RequiredCount = FMath::FloorToInt(TotalDistance / EachSpawnDistance);
	
	// Spawn new indicators if we need more
	while (PointingIndicators.Num() < RequiredCount)
	{
		AActor* NewIndicator = GetWorld()->SpawnActor<AActor>();
		NewIndicator->SetActorLabel("PointingIndicator");
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
		
		Mesh->SetStaticMesh(SphereMesh);
		Mesh->SetWorldScale3D(FVector(0.1f));
		Mesh->RegisterComponent();
		
		UMaterialInterface* OutlineSeeThrough = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/ActionStackMat/Mat_OutlineSeeThrough.Mat_OutlineSeeThrough"));
		Mesh->SetMaterial(0,  OutlineSeeThrough);
		
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
		FVector NewLocation = Start + MoveArmRef->ArmParts[2]->GetUpVector() * EachSpawnDistance * (i + 1);
		if (PointingIndicators[i])
		{
			PointingIndicators[i]->SetActorLocation(NewLocation);
		}
	}

	if (IsValid(InPointingAtItemActor)) InPointingAtItemActor->SetNeedOutlineMaterial(EMaterialOverlayStatus::Active);
}
