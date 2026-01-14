// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmPoint.h"
#include "Camera/CameraActor.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"
#include "Components/StaticMeshComponent.h"

UActionArmPoint* UActionArmPoint::Create(UObject* Outer, UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter)
{
	UActionArmPoint* Action = NewObject<UActionArmPoint>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->OwnerCharacter = InCharacter;
	return Action;
}

void UActionArmPoint::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
	
	FHitResult Hit;
	const FVector Start = OwnerCharacter->GetCameraActor()->GetActorLocation();
	const FVector End = Start + (OwnerCharacter->GetCameraActor()->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		PointingLocation = Hit.ImpactPoint;
		const FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
		AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, PointLocation));
	}
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
	FVector Start = OwnerCharacter->GetCameraActor()->GetActorLocation();
	FVector End = Start + (OwnerCharacter->GetCameraActor()->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		if (Hit.GetComponent()->GetOwner()->ActorHasTag("HandHeldItem"))
		{
			ActorPointedAt = Hit.GetComponent();
			RenderPointingToIndicator(PointingLocation,  Hit.GetComponent());
		}
		else
		{
			RenderPointingToIndicator(PointingLocation,  nullptr);
			ActorPointedAt = nullptr;
		}
		
		PointingLocation = Hit.ImpactPoint;
		const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
		FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
		MoveArmRef->TargetPose.Position = PointLocation;

		FVector LocationA = MoveArmRef->ArmParts[1]->GetComponentLocation();
		FVector LocationB = MoveArmRef->ArmParts[2]->GetComponentLocation();
		FVector Direction = LocationB - LocationA;
		 if (!Direction.IsNearlyZero())
        {
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
			FQuat RelativeQuat = Xform.InverseTransformRotation(NewRotation.Quaternion());
		 	MoveArmRef->TargetPose.Rotation = RelativeQuat;
        }
	}
	else
	{
		RenderPointingToIndicator(PointingLocation,  nullptr);
		ActorPointedAt = nullptr;
		const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
		FVector PointLocation = Xform.InverseTransformPosition(End);
		MoveArmRef->TargetPose.Position = PointLocation;
	}

	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}
	AnimTransforms.Empty();
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));

	FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, PointLocation));
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
}

bool UActionArmPoint::IsDone_Implementation() const
{
	return false;
}

void UActionArmPoint::RenderPointingToIndicator(const FVector& InHitLocation, const UActorComponent* InPointingAtItemActor)
{
	if (!MoveArmRef || MoveArmRef->ArmParts.Num() < 3) return;

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

	if (IsValid(InPointingAtItemActor))
	{
		UMaterialInterface* OutlineObject = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/ActionStackMat/Mat_OutlineObject.Mat_OutlineObject"));

		const AActor* ActorRef = InPointingAtItemActor->GetOwner();
		TArray<UStaticMeshComponent*> Components;
		ActorRef->GetComponents<UStaticMeshComponent>(Components);
		for (UStaticMeshComponent* StaticMeshComponent : Components)
		{
			if (StaticMeshComponent)
			{
				StaticMeshComponent->SetOverlayMaterial(OutlineObject);
			}
		}
	}
	else // Reset the last object that was outlined
	{
		if (IsValid(ActorPointedAt))
		{
			const AActor* ActorRef = ActorPointedAt->GetOwner();
			TArray<UStaticMeshComponent*> Components;
			ActorRef->GetComponents<UStaticMeshComponent>(Components);
			for (UStaticMeshComponent* StaticMeshComponent : Components)
			{
				if (StaticMeshComponent)
				{
					StaticMeshComponent->SetOverlayMaterial(nullptr);
				}
			}
		}
	}
}
