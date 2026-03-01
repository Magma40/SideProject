// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/BaseInteractableCharacter.h"

#include "CompanionPawn.h"
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
#include "WorldObjects/CompanionUIIndicator.h"

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

// TODO: Need to find better way to update pick up animations, tick is not optimized
// This will update an animation if a character tries to pick up item or drop an item
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
	
	//Update pick up or drop animation
	const int BeforeMoveNum = AnimTransforms.Num();
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
	MovingModel->GetRightArm()->TargetPose.Position = NewValues.Value.GetLocation();
	MovingModel->GetRightArm()->TargetPose.Rotation = NewValues.Value.GetRotation();
	AnimTransforms = NewValues.Key.Key;
	ElapsedTime = NewValues.Key.Value;

	//If character have finished the animation either pick up item or drop item in hand
	if (AnimTransforms.Num() % 2 == 0 && BeforeMoveNum != AnimTransforms.Num()) // Hard coding ):
	{
		if (IsValid(ItemInHand)) DropItemInHand();
		else PickUpItemToHand(ClosestObject);
	}
	else if (AnimTransforms.Num() < 2) // If there is nothing to animate, finish up
	{
		AnimTransforms.Empty();
		if (const ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(this))
		{
			CompanionPawn->GetCompanionWidgetIndicator()->SetNewIndicatorImage(ECompanionIndicatorImage::None);
		}
	}
}

//Will try to find the closest item to pick up or drop current item in hand
void ABaseInteractableCharacter::TryToPickUpItem()
{
	//If character has item in hand, see if character can drop it without getting blocked
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
			//Setup drop animation
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

	//Try to the closest item
	FVector HandLocation = MovingModel->GetRightHand()->GetComponentLocation();
	FPickupCandidate Best;
	
	TArray<AActor*> InteractableItems; //  Gather actors
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UInteractableItemInterface::StaticClass(), InteractableItems);
	for (AActor* Actor : InteractableItems)
	{
		AInteractableItemBase* InteractableItem = Cast<AInteractableItemBase>(Actor);
		if (InteractableItem == BackItemHolster) continue;
		if (InteractableItem->GetPersistentlyFrozen()) continue;
		float Dist = FVector::Dist(HandLocation, InteractableItem->GetActorLocation());
		if (Dist < Best.Distance)
		{
			Best.Object = InteractableItem;
			Best.Distance = Dist;
		}
	}

	//If we are able to pick up item
	if (Best.Object && Best.Distance <= MinDistanceToPickUpItem)
	{
		//Safety check to not pick up already referenced item
		if (ABaseInteractableCharacter* OwnerCharacter = Best.Object->GetOwnerCharacter())
		{
			if (Best.Object == OwnerCharacter->ItemInHand) OwnerCharacter->ItemInHand = nullptr;
			else if (Best.Object == OwnerCharacter->BackItemHolster) OwnerCharacter->BackItemHolster = nullptr;
		}

		//Setup of pick up animation
		ClosestObject = Best.Object;
		const FTransform Xform = Cast<USceneComponent>(MovingModel->GetRightArm())->GetComponentTransform();
		FVector HandLocationPos = Xform.InverseTransformPosition(MovingModel->GetRightHand()->GetComponentLocation());
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, HandLocationPos));
		const FVector LastKnowHandLocation = HandLocationPos;
		
		FVector ObjectLocalPos = Xform.InverseTransformPosition(ClosestObject->GetActorLocation());
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, ObjectLocalPos));
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, LastKnowHandLocation));

		if (const ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(this))
		{
			CompanionPawn->GetCompanionWidgetIndicator()->SetNewIndicatorImage(ECompanionIndicatorImage::ExclamationMark);
		}
	}
}

//Drop current item in hand
void ABaseInteractableCharacter::DropItemInHand()
{
	if (!IsValid(ItemInHand)) return;
	ItemInHand->DetachFromComponent_Implementation();
	ItemInHand->SetOwnerCharacter(nullptr);
	ItemInHand = nullptr;
}

//Pick up closest item
void ABaseInteractableCharacter::PickUpItemToHand(AInteractableItemBase* InSpecificPickUpItem)
{
	if (IsValid(InSpecificPickUpItem)) ClosestObject = InSpecificPickUpItem;
	if (!IsValid(ClosestObject)) return;
	ClosestObject->AttachToComponent_Implementation(MovingModel->GetRightHand());
	ClosestObject->SetOwnerCharacter(this);
	ItemInHand = ClosestObject;
	ClosestObject = nullptr;
}

//If the hand to holster distance is acceptable, try to either attach to back holster or deattach item from back holster
void ABaseInteractableCharacter::TryToInteractWithBackHolster()
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

//Attach item in hand to back holster
void ABaseInteractableCharacter::AttachToBackHolster()
{
	if (!IsValid(ItemInHand)) return;
	ItemInHand->AttachToComponent_Implementation(BackHolster);
	BackItemHolster = ItemInHand;
	ItemInHand = nullptr;
}

//Detach item from back holster to hand
void ABaseInteractableCharacter::DeattachFromBackHolster()
{
	PickUpItemToHand(BackItemHolster);
	BackItemHolster = nullptr;
}

//The character tries to interact with every interactable object in world
void ABaseInteractableCharacter::TryToInteractWithWorld()
{
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (!IsValid(InteractionManager)) return;

	//If character is close enough to interact with any interactable in world
	for (const auto Interaction : InteractionManager->GetInteractions(this))
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

//Respawns the character based on the current room spawn point
void ABaseInteractableCharacter::RespawnCharacter()
{
	TeleportTo(RoomRespawnSpawnPoint.GetLocation(), RoomRespawnSpawnPoint.Rotator());
}

