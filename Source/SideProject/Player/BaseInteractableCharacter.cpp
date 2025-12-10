// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BaseInteractableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MovingModel.h"

ABaseInteractableCharacter::ABaseInteractableCharacter() { PrimaryActorTick.bCanEverTick = true; }

void ABaseInteractableCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ChildrenActors;
	GetAllChildActors(ChildrenActors, true);
	for (AActor* FoundChild : ChildrenActors)
	{
		if (!FoundChild) continue;
		MovingModel = Cast<AMovingModel>(FoundChild);
	}
	TArray<USceneComponent*> AllComponents;
	MovingModel->GetComponents<USceneComponent>(AllComponents);
	for (USceneComponent* Comp : AllComponents)
	{
		if (!Comp) continue;
		if (Comp->GetName() == TEXT("BackItemHolster")) BackHolster = Comp;
	}
}

// Called every frame
void ABaseInteractableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseInteractableCharacter::ActionPickUpItem()
{
	if (AbleToPickUpItem())
	{
		if (TryToPickUpItem())
		{
			PickUpItemToHand();
			return true;
		}
	}
	return false;
}


bool ABaseInteractableCharacter::AbleToPickUpItem()
{
		if (IsValid(ClosestActor) || IsValid(ClosestComponent)) return false;
		return true;
}

bool ABaseInteractableCharacter::TryToPickUpItem() // Is this even needed?
{
	LastKnownClosestDistance = FLT_MAX;
	ClosestActor = nullptr;
	ClosestComponent = nullptr;
	
	TArray<AActor*> ActorsAround;
	UGameplayStatics::GetAllActorsWithTag (GetWorld(), "HandHeldItem", ActorsAround);
	if (ActorsAround.Num() > 0)
	{
		for(AActor* FoundActor : ActorsAround)
		{
			AActor*  NewClosestActor = GetClosestActorItem(FoundActor);
			if (NewClosestActor != nullptr) ClosestActor =NewClosestActor;
		}
	}

	TArray<USceneComponent*> ComponentsAround;
	GetAllComponentsWithTag(GetWorld(), "HandHeldItem", ComponentsAround);
	if (ComponentsAround.Num() > 0)
	{
		for(USceneComponent* FoundComponent : ComponentsAround)
		{
			USceneComponent*  NewClosestComponent = GetClosestComponentItem(FoundComponent);
			if (NewClosestComponent != nullptr) ClosestComponent = NewClosestComponent;
		}
	}
	
	if (IsValid(ClosestActor) && IsValid(ClosestComponent)) // This is getting too bloated
	{
		float ActorDistance =		 UE::Geometry::Distance(MovingModel->GetLeftHand()->GetComponentLocation(), ClosestActor->GetActorLocation());
		float ComponentDistance = UE::Geometry::Distance(MovingModel->GetLeftHand()->GetComponentLocation(), ClosestComponent->GetComponentLocation());
		if (ActorDistance < ComponentDistance)
		{
			ClosestComponent = nullptr;
			if (ActorDistance <= MinDistanceToPickUpItem) return true;
		}
		else if (ComponentDistance < ActorDistance)
		{
			ClosestActor = nullptr;
			if (ComponentDistance <= MinDistanceToPickUpItem) return true;
		}	
	}
	else if (IsValid(ClosestActor) && !IsValid(ClosestComponent))
	{
		float ActorDistance = UE::Geometry::Distance(MovingModel->GetLeftHand()->GetComponentLocation(), ClosestActor->GetActorLocation());
		if (ActorDistance <= MinDistanceToPickUpItem) return true;
	}
	else if (IsValid(ClosestComponent) && !IsValid(ClosestActor))
	{
		float ComponentDistance = UE::Geometry::Distance(MovingModel->GetLeftHand()->GetComponentLocation(), ClosestComponent->GetComponentLocation());
		if (ComponentDistance <= MinDistanceToPickUpItem) return true;
	}
	
	return false;
}

void ABaseInteractableCharacter::DropItemInHand()
{
	if (!GetCharacterMovement()->IsFalling() && (IsValid(ItemInHand)))
	{
		FVector Start = GetActorLocation() + FVector(0, 0, 50); 
		FVector End = Start + (GetActorForwardVector() * 100.0f);

		USceneComponent* ComponentItem = Cast<USceneComponent>(ItemInHand);
		AActor* ActorItem = Cast<AActor>(ItemInHand);
		
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		if (IsValid(ComponentItem))
		{
			CollisionParams.AddIgnoredComponent(Cast<UPrimitiveComponent>(ItemInHand));
		}
		else if (IsValid(ActorItem))
		{
			CollisionParams.AddIgnoredActor(ActorItem);
		}
		CollisionParams.bTraceComplex = true;
		
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility,  CollisionParams );
		if (!bHit) // Only drop if nothing was hit
		{
			if (IsValid(ActorItem) && !IsValid(ComponentItem))
			{
				USceneComponent* RootComp = ActorItem->GetRootComponent();
				if (!IsValid(RootComp)) return;
				
				ActorItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				ActorItem->SetActorLocation(End);
				if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(RootComp))
				{
					Prim->SetMobility(EComponentMobility::Movable);
					Prim->SetSimulatePhysics(true);
					Prim->SetEnableGravity(true);
					Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				}
				//EnablePhysicsRecursively(RootComp);
				ItemInHand = nullptr;
				ClosestActor = nullptr;
				ClosestComponent = nullptr;
			}
			else if (IsValid(ComponentItem) &&  !IsValid(ActorItem))
			{
				AActor* ComponentOwner = ComponentItem->GetOwner();
				USceneComponent* RootComp = ComponentOwner->GetRootComponent();
				if (!IsValid(ComponentOwner) || !IsValid(RootComp)) return;
				
				ComponentOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				ComponentOwner->SetActorLocation(End);
				if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(RootComp))
				{
					Prim->SetMobility(EComponentMobility::Movable);
					Prim->SetSimulatePhysics(true);
					Prim->SetEnableGravity(true);
					Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				}
				//EnablePhysicsRecursively(RootComp);
				ItemInHand = nullptr;
				ClosestActor = nullptr;
				ClosestComponent = nullptr;
			}
		}
	}
}
	
void ABaseInteractableCharacter::PickUpItemToHand()
{
	if (IsValid(ClosestActor) && !IsValid(ClosestComponent))
	{
		ItemInHand = ClosestActor;
		USceneComponent* RootComp = ClosestActor->GetRootComponent();
		if (!IsValid(RootComp)) return;
		
	    //DisablePhysicsRecursively(RootComp);
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(RootComp))
		{
			Prim->SetMobility(EComponentMobility::Movable);
			Prim->SetSimulatePhysics(false);
			Prim->SetEnableGravity(false);
			Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		ClosestActor->AttachToComponent(MovingModel->GetLeftHand(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else if (IsValid(ClosestComponent) &&  !IsValid(ClosestActor))
	{
		ItemInHand = ClosestComponent;
		AActor*ComponentOwner = ClosestComponent->GetOwner();
		USceneComponent* RootComp = ComponentOwner->GetRootComponent();
		if (!IsValid(ComponentOwner) || !IsValid(RootComp)) return;
		
		//DisablePhysicsRecursively(RootComp);
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(RootComp))
		{
			Prim->SetMobility(EComponentMobility::Movable);
			Prim->SetSimulatePhysics(false);
			Prim->SetEnableGravity(false);
			Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		ComponentOwner->AttachToComponent(MovingModel->GetLeftHand(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ABaseInteractableCharacter::TryToInteractWithBackHolster() // Doing this too fast will break it, resulting mostly with BackItemHolster being valid
{
	float Distance =	UE::Geometry::Distance(MovingModel->GetLeftHand()->GetComponentLocation(), BackHolster->GetComponentLocation());
	if (Distance > 100) return;
	if (IsValid(BackItemHolster) && !IsValid(ItemInHand))
	{
		DeattachToBackHolster();
	}
	else if (!IsValid(BackItemHolster) && IsValid(ItemInHand))
	{
		AttachToBackHolster();
	}
}

void ABaseInteractableCharacter::AttachToBackHolster()
{
	if (ItemInHand)
	{
		USceneComponent* ComponentItem = Cast<USceneComponent>(ItemInHand);
		AActor* ActorItem = Cast<AActor>(ItemInHand);
		if (IsValid(ComponentItem))
		{
			AActor*ComponentOwner = ComponentItem->GetOwner();
			USceneComponent* RootComp = ComponentOwner->GetRootComponent();
			if (!IsValid(ComponentOwner) || !IsValid(RootComp)) return;
		
			//DisablePhysicsRecursively(RootComp);
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(RootComp))
			{
				Prim->SetMobility(EComponentMobility::Movable);
				Prim->SetSimulatePhysics(false);
				Prim->SetEnableGravity(false);
				Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			ComponentOwner->AttachToComponent(BackHolster, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			BackItemHolster = ComponentOwner;
			ItemInHand = nullptr;
			ClosestActor = nullptr;
			ClosestComponent = nullptr;
		}
		else if (IsValid(ActorItem))
		{
			USceneComponent* RootComp = ActorItem->GetRootComponent();
			if (!IsValid(RootComp)) return;
		
			//DisablePhysicsRecursively(RootComp);
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(RootComp))
			{
				Prim->SetMobility(EComponentMobility::Movable);
				Prim->SetSimulatePhysics(false);
				Prim->SetEnableGravity(false);
				Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			ActorItem->AttachToComponent(BackHolster, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			BackItemHolster = ActorItem;
			ItemInHand = nullptr;
			ClosestActor = nullptr;
			ClosestComponent = nullptr;
		}
	}
}
void ABaseInteractableCharacter::DeattachToBackHolster()
{
	USceneComponent* ComponentItem = Cast<USceneComponent>(BackItemHolster);
	AActor* ActorItem = Cast<AActor>(BackItemHolster);
	if (IsValid(ActorItem)) ClosestActor = ActorItem;
	else if (IsValid(ComponentItem)) ClosestComponent = ComponentItem;
	BackItemHolster = nullptr;
	PickUpItemToHand();
}

AActor* ABaseInteractableCharacter::GetClosestActorItem(AActor* InActor)
{
	if (IsValid(InActor) && !InActor->IsHidden())
	{
		float DistanceToItem = GetDistanceTo(InActor);
		if (DistanceToItem < LastKnownClosestDistance)
		{
			LastKnownClosestDistance = DistanceToItem;
			return InActor;
		}
	}
	return nullptr;
}

USceneComponent* ABaseInteractableCharacter::GetClosestComponentItem(USceneComponent* InComponent)
{
	if (IsValid(InComponent))
	{
		float DistanceToItem =UE::Geometry::Distance(this->GetActorLocation(), InComponent->GetComponentLocation());
		if (DistanceToItem < LastKnownClosestDistance)
		{
			LastKnownClosestDistance = DistanceToItem;
			return InComponent;
		}
	}
	return nullptr;
}

UPrimitiveComponent* ABaseInteractableCharacter::GetPrimitiveFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;

	TArray<USceneComponent*> Components;
	Actor->GetComponents<USceneComponent>(Components);

	for (USceneComponent* Comp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
		{
			return Prim; // first found
		}

		// check children recursively
		TArray<USceneComponent*> Children;
		Comp->GetChildrenComponents(true, Children);
		for (USceneComponent* ChildComp : Children)
		{
			if (UPrimitiveComponent* PrimChild = Cast<UPrimitiveComponent>(ChildComp))
			{
				return PrimChild;
			}
		}
	}

	return nullptr;
}

void ABaseInteractableCharacter::DisablePhysicsRecursively(USceneComponent* Comp)
{
	if (!Comp) return;

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
	{
		Prim->SetMobility(EComponentMobility::Movable);
		Prim->SetSimulatePhysics(false);
		Prim->SetEnableGravity(false);
		Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TArray<USceneComponent*> Children;
	Comp->GetChildrenComponents(true, Children);
	for (USceneComponent* Child : Children)
	{
		DisablePhysicsRecursively(Child);
	}
}

void ABaseInteractableCharacter::EnablePhysicsRecursively(USceneComponent* Comp)
{
	if (!Comp) return;

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
	{
		Prim->SetMobility(EComponentMobility::Movable);
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	TArray<USceneComponent*> Children;
	Comp->GetChildrenComponents(true, Children);
	for (USceneComponent* Child : Children)
	{
		EnablePhysicsRecursively(Child);
	}
}

