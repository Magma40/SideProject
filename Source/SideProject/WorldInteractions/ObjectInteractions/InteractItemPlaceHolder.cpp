// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ObjectInteractions/InteractItemPlaceHolder.h"

#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "Tools/GenericCalls.h"
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"

UInteractItemPlaceHolder::UInteractItemPlaceHolder()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
	DistanceForInteraction = 200.0f;
}

UInteractItemPlaceHolder::~UInteractItemPlaceHolder(){}

void UInteractItemPlaceHolder::BeginPlay()
{
	Super::BeginPlay();
	
	UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
	if (IsValid(InteractionManager)) InteractionManager->AddInteraction(this);
}

void UInteractItemPlaceHolder::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
		if (InteractingCharacter->GetItemInHand() != ItemNeeded) return;
		if (FVector::Distance(InteractingCharacter->GetItemInHand()->GetActorLocation(), this->GetComponentLocation()) < DistanceForInteraction)
		{
			Interact_Implementation();
		}
	}
}

bool UInteractItemPlaceHolder::CanInteract_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	if (GetDistanceToInteraction_Implementation(InInteractingCharacter) > DistanceForInteraction) return false;
	return true;
}

float UInteractItemPlaceHolder::GetDistanceToInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	FVector ToDoorLocation = this->GetComponentLocation() - InInteractingCharacter->GetActorLocation();
	ToDoorLocation.Y = 0.0f;
	return ToDoorLocation.Size();
}

void UInteractItemPlaceHolder::PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	Super::PerformInteraction_Implementation(InInteractingCharacter);
	InteractingCharacter = InInteractingCharacter;
	if (IsValid(PlacedItem)) InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandToLocation(PlacedItem->GetActorLocation(), true);
	else InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandToLocation(this->GetComponentLocation(), false);
	Interacting = true;
}

void UInteractItemPlaceHolder::Interact_Implementation()
{
	Super::Interact_Implementation();
	if (!InteractingCharacter) return;
	
	if (IsValid(InteractingCharacter))
	{
		InteractingCharacter->GetItemInHand()->AttachToComponent_Implementation(this);
		PlacedItem = InteractingCharacter->GetItemInHand();
		InteractingCharacter->ResetItemInHand();
	}
    	
	if (IsValid(ToInteractActor))
	{
		UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ToInteractActor);
		if (IsValid(InteractObject)) InteractObject->Interact_Implementation();	
	}
	
 	if(bOneUsePlacement)
 	{
 		UInteractionManager* InteractionManager =  GetWorld()->GetSubsystem<UInteractionManager>();
    	if (IsValid(InteractionManager))
    	{
    		InteractionManager->RemoveInteraction(this);
    		DestroyComponent(false);
    	}
 	}
 	
	Interacting =  false;
}


