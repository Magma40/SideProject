#include "WorldInteractions/ObjectInteractions/InteractDoor.h"
#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Player/BaseInteractableCharacter.h"
#include "ProcedualAnimation/MovingModel.h"
#include "WorldInteractions/ItemInteractions/KeyItem.h"

UInteractDoor::UInteractDoor()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
}

void UInteractDoor::BeginPlay()
{
	TArray<AActor*> AttachedActors;
	GetOwner()->GetAttachedActors(AttachedActors);
	
	for (AActor* Child : AttachedActors)
	{
		if (!Child)
		{
			continue;
		}
		
		Lock = Child;
	}

	if (PhysicsConstraintsRef.ComponentProperty != "None")
	{
		UPhysicsConstraintComponent* PhysicsConstraints = Cast<UPhysicsConstraintComponent>(PhysicsConstraintsRef.GetComponent(GetOwner()));
		CachedDoorSwing = PhysicsConstraints->GetCurrentSwing1();
		PhysicsConstraints->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraints->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraints->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);	
	}
	
	Super::BeginPlay();
}

void UInteractDoor::TickComponent(const float DeltaTime, 
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bInteracting)
	{
		//UArmActionStack* ArmActionStack = InteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R();
		
		//TScriptInterface<IActionInterface> ActionInterfaceArmToLocation = ArmActionStack->GetInterfacedAction(ArmActionStack->GetAction<UActionArmHandToLocation>());
		
		 // if (!ArmActionStack->Contains(ActionInterfaceArmToLocation))
		 // {
		 // 	bInteracting = false;
		 // 	
		 // 	return;
		 // }
		
		if (!IsValid(InteractingCharacter->GetItemInHand()) || 
			InteractingCharacter->GetItemInHand() != ItemNeeded)
		{
			return;
		}
		
		if (FVector::Distance(
			InteractingCharacter->GetItemInHand()->GetActorLocation(), 
			Lock->GetActorLocation()) 
			< DistanceForInteraction / 10)
		{
			bInteracting = false;
			
			Interact_Implementation(InteractingCharacter);
		}
	}
}

void UInteractDoor::PerformInteraction_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	Super::PerformInteraction_Implementation(InInteractingCharacter);
	
	//InInteractingCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandToLocation(Lock->GetActorLocation(), false);
}

TArray<UStaticMeshComponent*> UInteractDoor::GetAllStaticMeshesInOwner_Implementation()
{
	TArray<UStaticMeshComponent*> FoundComponentsChildren;
	
	Lock->GetComponents<UStaticMeshComponent>(FoundComponentsChildren, true);
	
	return FoundComponentsChildren;
}

void UInteractDoor::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	if (IsValid(InInteractingCharacter) && IsValid(InInteractingCharacter->GetItemInHand()))
	{
		TArray<UStaticMeshComponent*> Meshes;
		Lock->GetComponents<UStaticMeshComponent>(Meshes);
		
		InInteractingCharacter->GetItemInHand()->AttachToComponent_Implementation(Meshes[0]);
		InInteractingCharacter->GetItemInHand()->SetPersistentlyFrozen(true);
		InInteractingCharacter->ResetItemInHand();
	}
	
	TArray<UPrimitiveComponent*> Primitives;
	Lock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Lock->GetComponents<UPrimitiveComponent>(Primitives);
	
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim))
		{
			continue;
		}
		
		if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Prim))
		{
			Mesh->SetOverlayMaterial(nullptr);
		}
		
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
	}
	
	if (PhysicsConstraintsRef.ComponentProperty != "None")
	{
		UPhysicsConstraintComponent* PhysicsConstraints = Cast<UPhysicsConstraintComponent>(PhysicsConstraintsRef.GetComponent(GetOwner()));
		PhysicsConstraints->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 110);
		PhysicsConstraints->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraints->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);	
	}
	
	Super::Interact_Implementation(InInteractingCharacter);
}


