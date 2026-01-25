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
		
		USceneComponent* ComponentItem = Cast<USceneComponent>(ItemInHand);
		AActor* ActorItem = Cast<AActor>(ItemInHand);
		AActor* OwnerActor = nullptr;
		USceneComponent* RootComp = nullptr;

		if (IsValid(ActorItem))
		{
			OwnerActor = ActorItem;
			RootComp = ActorItem->GetRootComponent();
		}
		else if (IsValid(ComponentItem))
		{
			OwnerActor = ComponentItem->GetOwner();
			RootComp = OwnerActor ? OwnerActor->GetRootComponent() : nullptr;
		}

		if (!IsValid(OwnerActor) || !IsValid(RootComp)) return;
		
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		CollisionParams.AddIgnoredActor(OwnerActor);
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
	
	TArray<AActor*> ActorsAround; //  Gather actors
	UGameplayStatics::GetAllActorsWithTag (GetWorld(), "HandHeldItem", ActorsAround);
	for (AActor* Actor : ActorsAround)
	{
		if (Actor == BackItemHolster) continue;
		float Dist = FVector::Dist(HandLocation, Actor->GetActorLocation());
		if (Dist < Best.Distance)
		{
			Best.Object = Actor;
			Best.Distance = Dist;
		}
	}

	TArray<USceneComponent*> ComponentsAround;  //  Gather components 
	GenericCalls::GetAllComponentsWithTag(GetWorld(), "HandHeldItem", ComponentsAround);
	for (USceneComponent* Comp : ComponentsAround)
	{
		if (Comp == BackItemHolster) continue;
		float Dist = FVector::Dist(HandLocation, Comp->GetComponentLocation());
		if (Dist < Best.Distance)
		{
			Best.Object = Comp;
			Best.Distance = Dist;
		}
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseInteractableCharacter::StaticClass(),  Actors);
	for (AActor* Actor : Actors)
	{
		ABaseInteractableCharacter* Character = Cast<ABaseInteractableCharacter>(Actor);
		if (Character == this) continue;
		if (Best.Object == Character->ItemInHand) Character->ItemInHand = nullptr;
		else if (Best.Object == BackItemHolster) Character->BackItemHolster = nullptr;
	}
	
	if (Best.Object && Best.Distance <= MinDistanceToPickUpItem)
	{
		ClosestObject = Best.Object;
		const FTransform Xform = Cast<USceneComponent>(MovingModel->GetRightArm())->GetComponentTransform();
		FVector HandLocationPos = Xform.InverseTransformPosition(MovingModel->GetRightHand()->GetComponentLocation());
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, HandLocationPos));
		const FVector LastKnowHandLocation = HandLocationPos;

		USceneComponent* ComponentItem = Cast<USceneComponent>(ClosestObject);
		AActor* ActorItem = Cast<AActor>(ClosestObject);
		AActor* OwnerActor = nullptr;
		
		if (IsValid(ActorItem)) OwnerActor = ActorItem;
		else if (IsValid(ComponentItem)) OwnerActor = ComponentItem->GetOwner();
		
		FVector ObjectLocalPos = Xform.InverseTransformPosition(OwnerActor->GetActorLocation());
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, ObjectLocalPos));
		AnimTransforms.Add(FTransform(MovingModel->GetRightArm()->TargetPose.Rotation, LastKnowHandLocation));
	}
}

void ABaseInteractableCharacter::DropItemInHand()
{
	if (!IsValid(ItemInHand)) return;

	USceneComponent* ComponentItem = Cast<USceneComponent>(ItemInHand);
	AActor* ActorItem = Cast<AActor>(ItemInHand);
	AActor* OwnerActor = nullptr;
	USceneComponent* RootComp = nullptr;

	if (IsValid(ActorItem))
	{
		OwnerActor = ActorItem;
		RootComp = ActorItem->GetRootComponent();
	}
	else if (IsValid(ComponentItem))
	{
		OwnerActor = ComponentItem->GetOwner();
		RootComp = OwnerActor ? OwnerActor->GetRootComponent() : nullptr;
	}

	if (!IsValid(OwnerActor) || !IsValid(RootComp)) return;
	OwnerActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	TArray<UPrimitiveComponent*> Primitives;
	OwnerActor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim)) continue;
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	ItemInHand = nullptr;
}
	
void ABaseInteractableCharacter::PickUpItemToHand(UObject* InSpecificPickUpItem)
{
	if (IsValid(InSpecificPickUpItem)) ClosestObject = InSpecificPickUpItem;
	if (!IsValid(ClosestObject)) return;

	const USceneComponent* ComponentItem = Cast<USceneComponent>(ClosestObject);
	const AActor* ActorItem = Cast<AActor>(ClosestObject);
	const AActor* OwnerActor = nullptr;
	USceneComponent* RootComp = nullptr;

	if (IsValid(ActorItem))
	{
		OwnerActor = ActorItem;
		RootComp = ActorItem->GetRootComponent();
	}
	else if (IsValid(ComponentItem))
	{
		OwnerActor = ComponentItem->GetOwner();
		RootComp = OwnerActor ? OwnerActor->GetRootComponent() : nullptr;
	}

	if (!IsValid(OwnerActor) || !IsValid(RootComp)) return;

	TArray<UPrimitiveComponent*> Primitives;
	OwnerActor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim)) continue;
		Prim->SetSimulatePhysics(false);
		Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TArray<UStaticMeshComponent*> Components;
	OwnerActor->GetComponents<UStaticMeshComponent>(Components);
	for (UStaticMeshComponent* StaticMeshComponent : Components)
	{
		if (StaticMeshComponent)
		{
			StaticMeshComponent->SetOverlayMaterial(nullptr);
		}
	}
	
	OwnerActor->GetRootComponent()->AttachToComponent(MovingModel->GetRightHand(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ItemInHand = ClosestObject;
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

	const USceneComponent* ComponentItem = Cast<USceneComponent>(ItemInHand);
	AActor* ActorItem = Cast<AActor>(ItemInHand);
	AActor* OwnerActor = nullptr;

	if (IsValid(ActorItem)) OwnerActor = ActorItem;
	else if (IsValid(ComponentItem)) OwnerActor = ComponentItem->GetOwner();

	OwnerActor->AttachToComponent(BackHolster, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	BackItemHolster = OwnerActor;
	ItemInHand = nullptr;
}
void ABaseInteractableCharacter::DeattachFromBackHolster()
{
	PickUpItemToHand(BackItemHolster);
	BackItemHolster = nullptr;
}

void ABaseInteractableCharacter::TryToInteractWithWorld()
{
	UInteractionManager* InteractionManager = UInteractionManager::GetMain(GetWorld());
	if (!IsValid(InteractionManager)) return;

	for (auto Interaction : InteractionManager->GetInteractions(this))
	{
		UInteractionBehaviour* ActionRef = Cast<UInteractionBehaviour>(Interaction.GetObject());
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

