// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/InteractionManager.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"

void UInteractionManager::AddInteraction(UObject* Interaction)
{
	if (!IsValid(Interaction) && !IsValid(this)) return;
	if(Contains(Interaction)) return;
	UE_LOG(LogTemp, Warning,TEXT("Interaction %s Has Been Added Onto The %s"),*Interaction->GetFName().ToString(), *StaticClass()->GetFName().ToString());
	Interactions.Add(Interaction);
}

void UInteractionManager::RemoveInteraction(const UObject* Interaction)
{
	if (!IsValid(Interaction) && !IsValid(this)) return;
	if(!Contains(Interaction)) return;
	UE_LOG(LogTemp, Error,TEXT("Interaction %s Has Been Removed From The %s"),*Interaction->GetFName().ToString(), *StaticClass()->GetFName().ToString());
	Interactions.Remove(Interaction);
}

TSet<UObject*>  UInteractionManager::GetInteractions(ABaseInteractableCharacter* InteractingCharacter)
{
    TSet<UObject*> Results;
	for (UObject* Obj : Interactions)
	{
		if (!IsValid(Obj)) continue;
		if (!Obj->Implements<UInteractionInterface>()) continue;

		const float Distance =IInteractionInterface::Execute_GetDistanceToInteraction(Obj, InteractingCharacter);
		const float MaxDistance =IInteractionInterface::Execute_GetInteractionDistance(Obj);

		if (Distance < MaxDistance)
		{
			Results.Add(Obj);
		}
	}
	return Results;
}

//UInteractionBehaviour

void UInteractionBehaviour::BeginPlay()
{
	Super::BeginPlay();
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (IsValid(InteractionManager)) InteractionManager->AddInteraction(this);
}

void UInteractionBehaviour::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	switch (MaterialOverlayStatus)
	{
		case EMaterialOverlayStatus::None: break;
		case EMaterialOverlayStatus::Active: { EnableMaterialInterface_Implementation(); break; }
		case EMaterialOverlayStatus::Inactive: { DisableMaterialInterface_Implementation(); break; }
	}
}

void UInteractionBehaviour::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	if (IsValid(ActorToCallInteract))
	{
		UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ActorToCallInteract);
		if (IsValid(InteractObject)) InteractObject->Interact_Implementation(InInteractingCharacter);

		if (ActorToCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			Cast<IInteractionInterface>(ActorToCallInteract)->Interact_Implementation(InInteractingCharacter);
		}
	}
	
	if (bOneUse)
	{
		UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
		if (IsValid(InteractionManager))
		{
			InteractionManager->RemoveInteraction(this);
			DestroyComponent(false);
		}	
	}
	else bInteracting =  false;
}

bool UInteractionBehaviour::CanInteract_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	const float Dist = GetDistanceToInteraction_Implementation(InInteractingCharacter);
	return Dist <= DistanceForInteraction;
}

float UInteractionBehaviour::GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	FVector ToInteractionLocation = this->GetComponentLocation() - InInteractingCharacter->GetActorLocation();
	ToInteractionLocation.Z = 0.0f; // ignore vertical difference
	return ToInteractionLocation.Size();
}

void UInteractionBehaviour::PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	if (bInteracting) return;
	if (IsValid(InInteractingCharacter))
	{
		InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->ClearAllActionsExceptIndex(1);
	}
	bInteracting = true;
	InteractingCharacter = InInteractingCharacter;
}

TArray<UStaticMeshComponent*>  UInteractionBehaviour::GetAllStaticMeshesInOwner_Implementation()
{
	TArray<UStaticMeshComponent*> FoundComponentsChildren;
	
	//Get Parent and its components
	AActor* ParentOwner = GetAttachParentActor();
	if (!IsValid(ParentOwner)) return FoundComponentsChildren; 
	TArray<AActor*> FoundActorsChildren;
	ParentOwner->GetAllChildActors(FoundActorsChildren, true);

	//Check if found ActorChildren have components
	ParentOwner->GetComponents<UStaticMeshComponent>(FoundComponentsChildren, true);
	for (AActor* ActorChild : FoundActorsChildren)
	{
		TArray<UStaticMeshComponent*> MoreFoundComponentsChildren;
		ActorChild->GetComponents<UStaticMeshComponent>(MoreFoundComponentsChildren, true);
		for (UStaticMeshComponent* StaticMeshChild : MoreFoundComponentsChildren)
		{
			if (IsValid(StaticMeshChild) && !FoundComponentsChildren.Contains(StaticMeshChild)) continue;
			FoundComponentsChildren.Add(StaticMeshChild);
		}
	}
	return FoundComponentsChildren;
}

void UInteractionBehaviour::EnableMaterialInterface_Implementation()
{
	//Get Parent and its components
	TArray<UStaticMeshComponent*> FoundComponentsChildren = GetAllStaticMeshesInOwner_Implementation();
	for (UStaticMeshComponent* StaticMeshChild : FoundComponentsChildren)
	{
		if (IsValid(StaticMeshChild))
		{
			StaticMeshChild->SetOverlayMaterial(OutlineMaterialInterface);
		}
	}
	MaterialOverlayStatus = EMaterialOverlayStatus::Inactive;
}

void UInteractionBehaviour::DisableMaterialInterface_Implementation()
{
	TArray<UStaticMeshComponent*> FoundComponentsChildren = GetAllStaticMeshesInOwner_Implementation();
	for (UStaticMeshComponent* StaticMeshChild : FoundComponentsChildren)
	{
		if (IsValid(StaticMeshChild))
		{
			StaticMeshChild->SetOverlayMaterial(nullptr);
		}
	}
	MaterialOverlayStatus = EMaterialOverlayStatus::None;
}


