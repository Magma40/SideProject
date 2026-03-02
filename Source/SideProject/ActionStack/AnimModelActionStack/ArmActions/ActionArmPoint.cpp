// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CompanionPawn.h"
#include "Tools/GenericCalls.h"
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"
#include "WorldObjects/PlayerCamera.h"
#include "WorldInteractions/InteractionManager.h"
#include "WorldObjects/CompanionUIIndicator.h"
#include "WorldObjects/PointingIndicator.h"

UActionArmPoint* UActionArmPoint::Create(UObject* Outer, UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter, ABaseInteractableCharacter* InCompanion )
{
	UActionArmPoint* Action = NewObject<UActionArmPoint>(Outer);
	Action->MoveArmRef = InMoveArm;
	Action->OwnerCharacter = InCharacter;
	Action->OwnerCompanion = InCompanion;
	Action->TheMainCameraInScene = APlayerCamera::GetCamera(InCharacter->GetWorld());
	return Action;
}

void UActionArmPoint::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (const ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCompanion))
	{
		CompanionPawn->GetCompanionWidgetIndicator()->SetNewIndicatorImage(ECompanionIndicatorImage::QuestionMark);
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
	if (const ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCompanion))
	{
		CompanionPawn->GetCompanionWidgetIndicator()->SetNewIndicatorImage(ECompanionIndicatorImage::None);
	}
}

void UActionArmPoint::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);

	//Render indicator points from hand to pointing position
	RenderPointingToIndicator();

	//If a character is pointing at an interactable object, get that actor
	GetActorPointingAt();

	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FQuat RelativeQuat;

	FVector LocationA = MoveArmRef->ArmParts[1]->GetComponentLocation();
	FVector LocationB = MoveArmRef->ArmParts[2]->GetComponentLocation();

	//Calculate which direction the arm should point at
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

	//Make it so the "top" of hand is facing the direction of the pointing location
	FVector NewYAxis = FVector::CrossProduct(NewZAxis, NewXAxis).GetSafeNormal();
	FMatrix Matrix( FPlane(NewXAxis, 0.f), FPlane(NewYAxis, 0.f), FPlane(NewZAxis, 0.f), FPlane(0.f, 0.f, 0.f, 1.f));
	FRotator NewRotation = Matrix.Rotator();
	RelativeQuat = Xform.InverseTransformRotation(NewRotation.Quaternion());

	//Construct animation points to point arm at pointing location
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

	//Update arm pointing animation 
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
}

bool UActionArmPoint::IsDone_Implementation() const
{
	return false;
}

void UActionArmPoint::RenderPointingToIndicator()
{
	if (!MoveArmRef || MoveArmRef->ArmParts.Num() < 3) return;
	FHitResult Hit;
	const FVector StartCamera = TheMainCameraInScene->GetActorLocation();
	const FVector End = StartCamera + (TheMainCameraInScene->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "InvisibleWall", ActorsToIgnore);
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(TheMainCameraInScene);
	Params.AddIgnoredActors(ActorsToIgnore);
	if (GetWorld()->LineTraceSingleByChannel(Hit,StartCamera,End,ECC_Visibility,Params ))
	{
		PointingLocation = Hit.ImpactPoint;
		constexpr float EachSpawnDistance = 25.0f;
		const FVector Start = MoveArmRef->ArmParts[2]->GetComponentLocation();

		const float TotalDistance = FVector::Distance(Hit.ImpactPoint, Start);
		const int32 RequiredCount = FMath::FloorToInt(TotalDistance / EachSpawnDistance);
	
		// Spawn new indicators if we need more
		while (PointingIndicators.Num() < RequiredCount)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerCharacter;
			SpawnParams.Instigator = OwnerCharacter;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			APointingIndicator* Indicator = GetWorld()->SpawnActor<APointingIndicator>(APointingIndicator::StaticClass(), OwnerCharacter->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

			if (IsValid(Indicator))
			{
				PointingIndicators.Add(Indicator);
			}
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
	}
}

void UActionArmPoint::GetActorPointingAt()
{
	if (!MoveArmRef || MoveArmRef->ArmParts.Num() < 3) return;
	
	FHitResult Hit;
	const FVector Start = MoveArmRef->ArmParts[2]->GetComponentLocation();
	FVector End = Start + MoveArmRef->ArmParts[2]->GetUpVector() * 10000.0f;
	
	FCollisionQueryParams Params;
	TArray<AActor*> ChildActors;
	OwnerCharacter->GetAllChildActors(ChildActors);
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActors(ChildActors);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (IsValid(HitActor))
		{
			UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(HitActor);
			if (IsValid(InteractObject))
			{
				InteractObject->SetNeedOutlineMaterial_Implementation(EMaterialOverlayStatus::Active);
				ActorPointedAt = HitActor;
			}
			else if (AInteractableItemBase* InteractableItem = Cast<AInteractableItemBase>(HitActor))
			{
				if (!InteractableItem->GetPersistentlyFrozen())
				{
					ActorPointedAt = HitActor;
					InteractableItem->SetNeedOutlineMaterial_Implementation(EMaterialOverlayStatus::Active);
				}
				else ActorPointedAt = nullptr;
			}
			else ActorPointedAt = nullptr;
		}
	}
}
