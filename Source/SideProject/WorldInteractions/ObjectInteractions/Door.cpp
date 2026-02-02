// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ObjectInteractions/Door.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "WorldInteractions/ItemInteractions/KeyItem.h"

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
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (IsValid(InteractionManager)) InteractionManager->AddInteraction(this);
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
		if ( !Cast<AKeyItem>(InteractingCharacter->GetItemInHand())) return;
		if (FVector::Distance(InteractingCharacter->GetItemInHand()->GetActorLocation(), Lock->GetActorLocation()) < 50.0f)
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
	TArray<UStaticMeshComponent*> Meshes;
	Lock->GetComponents<UStaticMeshComponent>(Meshes);
	InteractingCharacter->GetItemInHand()->AttachToComponent_Implementation(Meshes[0]);
	InteractingCharacter->GetItemInHand()->AddActorLocalOffset(FVector(0.0f, -250.0f, 0.0f));
	InteractingCharacter->ResetItemInHand();
	bIsLocked = false;
	
	TArray<UPrimitiveComponent*> Primitives;
	Lock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Lock->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim)) continue;
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
	}
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (IsValid(InteractionManager))
	{
		InteractionManager->RemoveInteraction(this);
		DestroyComponent(false);
	}
}


