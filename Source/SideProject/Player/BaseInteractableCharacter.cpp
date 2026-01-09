// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BaseInteractableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "VectorTypes.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"

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

	AllComponents.Empty();
	this->GetComponents<USceneComponent>(AllComponents);
	for (USceneComponent* Comp : AllComponents)
	{
		if (!Comp) continue;
		CompanionActionStack = Cast<UCompanionActionStack>(Comp);
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
			PickUpItemToHand(nullptr);
			return true;
		}
	}
	return false;
}


bool ABaseInteractableCharacter::AbleToPickUpItem()
{
		if (IsValid(ItemInHand)) return false;
		return true;
}

bool ABaseInteractableCharacter::TryToPickUpItem()
{
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
		return true;
	}
	return false;
}

void ABaseInteractableCharacter::DropItemInHand()
{
	if (!IsValid(ItemInHand)) return;
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

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility,  CollisionParams );
	if (!bHit) // Only drop if nothing was hit
	{
		OwnerActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OwnerActor->SetActorLocation(End);
		TArray<UPrimitiveComponent*> Primitives;
		OwnerActor->GetComponents<UPrimitiveComponent>(Primitives);
		for (UPrimitiveComponent* Prim : Primitives)
		{
			if (!IsValid(Prim)) continue;
			//Prim->SetSimulatePhysics(true);
			//Prim->SetEnableGravity(true);
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		ItemInHand = nullptr;
	}
}
	
void ABaseInteractableCharacter::PickUpItemToHand(UObject* inSpecificPickUpItem)
{
	if (IsValid(inSpecificPickUpItem)) ClosestObject = inSpecificPickUpItem;
	if (!IsValid(ClosestObject)) return;
	
	USceneComponent* ComponentItem = Cast<USceneComponent>(ClosestObject);
	AActor* ActorItem = Cast<AActor>(ClosestObject);
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

	TArray<UPrimitiveComponent*> Primitives;
	OwnerActor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim)) continue;
		Prim->SetSimulatePhysics(false);
		//Prim->SetEnableGravity(false);
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
	float Distance =	UE::Geometry::Distance(MovingModel->GetRightHand()->GetComponentLocation(), BackHolster->GetComponentLocation());
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

	USceneComponent* ComponentItem = Cast<USceneComponent>(ItemInHand);
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

void ABaseInteractableCharacter::Jump() 
{
	Super::Jump();
	if (MovingModel == nullptr || GetMovementComponent()->IsFalling()) return;
	MovingModel->SetLegsJumpingAnim();
}

