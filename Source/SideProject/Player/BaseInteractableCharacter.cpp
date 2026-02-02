// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/BaseInteractableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "WorldInteractions/InteractionManager.h"
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"

ABaseInteractableCharacter::ABaseInteractableCharacter() { PrimaryActorTick.bCanEverTick = true; }

void ABaseInteractableCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get MovingModel
	MovingModel =  GenericCalls::FindChildActorByClass<AMovingModel>(this);
	if (!MovingModel)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - MovingModel not found or wrong type"), *StaticClass()->GetName());
		return;
	}

	// Get BackItemHolster
	BackHolster = GenericCalls::FindChildByExactName(MovingModel, TEXT("BackItemHolster"));
	if (!BackHolster)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - BackHolster not found or wrong type"), *StaticClass()->GetName());
		return;
	}
}

// Called every frame
void ABaseInteractableCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AnimTransforms.IsEmpty()) return;

	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - Tick - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}
	const int BeforeMoveNum = AnimTransforms.Num();
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MovingModel->GetRightArm()->TargetPose.Position = NewValues.Value.GetLocation();
	MovingModel->GetRightArm()->TargetPose.Rotation = NewValues.Value.GetRotation();
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;

	if (AnimTransforms.Num() % 2 == 0 && BeforeMoveNum != AnimTransforms.Num()) // Hard coding ):
	{
		if (IsValid(ItemInHand)) DropItemInHand();
		else PickUpItemToHand(ClosestObject);
	}
	else if (AnimTransforms.Num() < 2) 	AnimTransforms.Empty();
}

void ABaseInteractableCharacter::TryToPickUpItem()
{
	if (IsValid(ItemInHand))
	{
		if (GetCharacterMovement()->IsFalling()) return;
		FVector Start = GetActorLocation() + FVector(0, 0, 50); 
		FVector End = Start + (GetActorForwardVector() * 100.0f);
		
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		CollisionParams.AddIgnoredActor(ItemInHand);
		CollisionParams.bTraceComplex = true;

		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility,  CollisionParams );
		if (!bHit) // Only drop if nothing was hit
		{
			const FTransform Xform = Cast<USceneComponent>(MovingModel->GetRightArm())->GetComponentTransform();
			FVector HandLocationPos = Xform.InverseTransformPosition(MovingModel->GetRightHand()->GetComponentLocation());
			AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, HandLocationPos));
			const FVector LastKnowHandLocation = HandLocationPos;

			FVector DropLocation = GetActorLocation() + FVector(0, 0, 50) + (GetActorForwardVector() * 100.0f);
			FVector DropLocalLocation = Xform.InverseTransformPosition(DropLocation);
			AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, DropLocalLocation));
			AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, LastKnowHandLocation));
		}
		return; 
	}
	
	FVector HandLocation = MovingModel->GetRightHand()->GetComponentLocation();
	FPickupCandidate Best;
	
	TArray<AActor*> InteractableItems; //  Gather actors
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UInteractableItemInterface::StaticClass(), InteractableItems);
	for (AActor* Actor : InteractableItems)
	{
		AInteractableItemBase* InteractableItem = Cast<AInteractableItemBase>(Actor);
		if (InteractableItem == BackItemHolster) continue;
		float Dist = FVector::Dist(HandLocation, InteractableItem->GetActorLocation());
		if (Dist < Best.Distance)
		{
			Best.Object = InteractableItem;
			Best.Distance = Dist;
		}
	}
	
	if (Best.Object && Best.Distance <= MinDistanceToPickUpItem)
	{
		if (ABaseInteractableCharacter* OwnerCharacter = Best.Object->GetOwnerCharacter())
		{
			if (Best.Object == OwnerCharacter->ItemInHand) OwnerCharacter->ItemInHand = nullptr;
			else if (Best.Object == OwnerCharacter->BackItemHolster) OwnerCharacter->BackItemHolster = nullptr;
		}
		
		ClosestObject = Best.Object;
		const FTransform Xform = Cast<USceneComponent>(MovingModel->GetRightArm())->GetComponentTransform();
		FVector HandLocationPos = Xform.InverseTransformPosition(MovingModel->GetRightHand()->GetComponentLocation());
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, HandLocationPos));
		const FVector LastKnowHandLocation = HandLocationPos;
		
		FVector ObjectLocalPos = Xform.InverseTransformPosition(ClosestObject->GetActorLocation());
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, ObjectLocalPos));
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, LastKnowHandLocation));
	}
}

void ABaseInteractableCharacter::DropItemInHand()
{
	if (!IsValid(ItemInHand)) return;
	ItemInHand->DetachFromComponent_Implementation();
	ItemInHand->SetOwnerCharacter(nullptr);
	ItemInHand = nullptr;
}
	
void ABaseInteractableCharacter::PickUpItemToHand(AInteractableItemBase* InSpecificPickUpItem)
{
	if (IsValid(InSpecificPickUpItem)) ClosestObject = InSpecificPickUpItem;
	if (!IsValid(ClosestObject)) return;
	ClosestObject->AttachToComponent_Implementation(MovingModel->GetRightHand());
	ClosestObject->SetOwnerCharacter(this);
	ItemInHand = ClosestObject;
	ClosestObject = nullptr;
}

void ABaseInteractableCharacter::TryToInteractWithBackHolster() // Doing this too fast will break it, resulting mostly with BackItemHolster being valid
{
	const float Distance =	UE::Geometry::Distance(MovingModel->GetRightHand()->GetComponentLocation(), BackHolster->GetComponentLocation());
	if (Distance > 50) return;
	if (IsValid(BackItemHolster) && !IsValid(ItemInHand))
	{
		DeattachFromBackHolster();
	}
	else if (!IsValid(BackItemHolster) && IsValid(ItemInHand))
	{
		AttachToBackHolster();
	}
}

void ABaseInteractableCharacter::AttachToBackHolster()
{
	if (!IsValid(ItemInHand)) return;
	ItemInHand->AttachToComponent_Implementation(BackHolster);
	BackItemHolster = ItemInHand;
	ItemInHand = nullptr;
}
void ABaseInteractableCharacter::DeattachFromBackHolster()
{
	PickUpItemToHand(BackItemHolster);
	BackItemHolster = nullptr;
}

void ABaseInteractableCharacter::TryToInteractWithWorld()
{
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (!IsValid(InteractionManager)) return;

	for (auto Interaction : InteractionManager->GetInteractions(this))
	{
		UInteractionBehaviour* ActionRef = Cast<UInteractionBehaviour>(Interaction);
		if (ActionRef->CanInteract_Implementation(this))
		{
			ActionRef->PerformInteraction_Implementation(this);
		}
	}
}

void ABaseInteractableCharacter::Jump() 
{
	Super::Jump();
	if (MovingModel == nullptr || GetMovementComponent()->IsFalling()) return;
	MovingModel->SetJumpingAnim();
}

