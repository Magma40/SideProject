// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ObjectInteractions/Door.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"

//__________ ADoor __________
UDoor::UDoor()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
	DistanceForInteraction = 200.0f;
}

UDoor::~UDoor(){}

void UDoor::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> AttachedActors;
	GetOwner()->GetAttachedActors(AttachedActors);
	for (AActor* Child : AttachedActors)
	{
		if (!Child) continue;
		Lock = Child;
	}
	UInteractionManager::GetMain(GetOwner()->GetWorld())->AddInteraction(this);
}

void UDoor::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (Interacting)
	{
		UArmActionStack* ArmActionStack = InteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R();
		const TScriptInterface<IActionInterface> ActionInterfaceArmToLocation = ArmActionStack->GetInterfacedAction(ArmActionStack->GetAction<UActionArmHandToLocation>());
		if (!ArmActionStack->Contains(ActionInterfaceArmToLocation))
		{
			Interacting = false;
			return;
		}
		
		if (!IsValid(InteractingCharacter->GetItemInHand())) return;
		if ( !InteractingCharacter->GetItemInHand()->IsA(ItemNeeded)) return;
		const USceneComponent* ComponentItem = Cast<USceneComponent>(InteractingCharacter->GetItemInHand());
		const AActor* ActorItem = Cast<AActor>(InteractingCharacter->GetItemInHand());
		const AActor* OwnerActor = nullptr;

		if (IsValid(ActorItem)) OwnerActor = ActorItem;
		else if (IsValid(ComponentItem)) OwnerActor = ComponentItem->GetOwner();
		
		if (FVector::Distance(OwnerActor->GetActorLocation(), Lock->GetActorLocation()) < 50.0f)
		{
			Interacting = false;
			Unlock();
		}
	}
}

bool UDoor::CanInteract_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	if (GetDistanceToInteraction_Implementation(InInteractingCharacter) > DistanceForInteraction) return false;
	return true;
}

float UDoor::GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	FVector ToDoorLocation = this->GetComponentLocation() - InInteractingCharacter->GetActorLocation();
	ToDoorLocation.Y = 0.0f;
	return ToDoorLocation.Size();
}

void UDoor::PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	Super::PerformInteraction_Implementation(InInteractingCharacter);
	InteractingCharacter = InInteractingCharacter;
	InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandToLocation(Lock->GetActorLocation());
	Interacting = true;
}

void UDoor::Unlock()
{
	const USceneComponent* ComponentItem = Cast<USceneComponent>(InteractingCharacter->GetItemInHand());
	const AActor* ActorItem = Cast<AActor>(InteractingCharacter->GetItemInHand());
	const AActor* OwnerActor = nullptr;

	if (IsValid(ActorItem)) OwnerActor = ActorItem;
	else if (IsValid(ComponentItem)) OwnerActor = ComponentItem->GetOwner();

	TArray<UPrimitiveComponent*> Primitives;
	OwnerActor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim)) continue;
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
	}
	OwnerActor->GetRootComponent()->AttachToComponent(Lock->GetRootComponent() ,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	InteractingCharacter->ResetItemInHand();

	Lock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Lock->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim)) continue;
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
	}
	UInteractionManager::GetMain(GetOwner()->GetWorld())->RemoveInteraction(this);
	DestroyComponent(false);	
}


