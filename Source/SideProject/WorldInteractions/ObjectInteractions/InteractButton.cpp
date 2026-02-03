// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ObjectInteractions/InteractButton.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"

UInteractButton::UInteractButton()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
	DistanceForInteraction = 200.0f;
}

UInteractButton::~UInteractButton(){}

void UInteractButton::BeginPlay()
{
	Super::BeginPlay();
	
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (IsValid(InteractionManager)) InteractionManager->AddInteraction(this);
}

bool UInteractButton::CanInteract_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	if (GetDistanceToInteraction_Implementation(InInteractingCharacter) > DistanceForInteraction) return false;
	return true;
}

float UInteractButton::GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	FVector ToButtonLocation = this->GetComponentLocation() - InInteractingCharacter->GetActorLocation();
	ToButtonLocation.Y = 0.0f;
	return ToButtonLocation.Size();
}

void UInteractButton::PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	Super::PerformInteraction_Implementation(InInteractingCharacter);
	Interacting = true;
	InteractingCharacter = InInteractingCharacter;
	InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandToLocation(GetComponentLocation(), false);
}

void UInteractButton::Interact_Implementation()
{
	if (!InteractingCharacter) return;
	if (IsValid(ToInteractActor))
	{
		UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ToInteractActor);
		if (IsValid(InteractObject)) InteractObject->Interact_Implementation();	
	}
	Interacting =  false;
	
	if (bOneUseButton)
	{
		UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
		if (IsValid(InteractionManager))
		{
			InteractionManager->RemoveInteraction(this);
			DestroyComponent(false);
		}	
	}
}


