#include "InteractionMovingFence.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "ActionStack/CameraActionStack/Actions/ActionCameraDefaultMovement.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/GenericCalls.h"
#include "WorldObjects/PlayerCamera.h"

AInteractionMovingFence::AInteractionMovingFence()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractionMovingFence::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(ActorToCallInteract))
	{
		return;
	}
	
	if (ActorToCallInteract->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		ScriptInterface.SetObject(ActorToCallInteract);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(ActorToCallInteract));

		InteractionToCallInteract = ScriptInterface;
		
		return;
	}

	UInteractionBehaviour* InteractObject = GenericCalls::FindChildComponentByClass<UInteractionBehaviour>(ActorToCallInteract);
	
	if (IsValid(InteractObject) && 
		InteractObject->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		TScriptInterface<IInteractionInterface> ScriptInterface;
		
		ScriptInterface.SetObject(InteractObject);
		ScriptInterface.SetInterface(Cast<IInteractionInterface>(InteractObject));
		
		InteractionToCallInteract = ScriptInterface;
	}
}

void AInteractionMovingFence::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	USceneComponent* ComponentsToMove = Cast<USceneComponent>(ComponentsToMoveRef.GetComponent(this));
	
	if (!IsValid(ComponentsToMove))
	{
		return;
	}

	if (AnimTransforms.Num() > 1)
	{
		const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
		
		if (!AnimSubsystem)
		{
			UE_LOG(LogTemp, Error,TEXT("%s - Tick - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
			return;
		}
		
		const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
			AnimSubsystem->CustomAnim(DeltaTime, 
				AnimTransforms,
				ElapsedTime, 
				PlayRate);
		
		ComponentsToMove->SetRelativeLocation(NewValues.Value.GetLocation());
		
		AnimTransforms = NewValues.Key.Key;
		ElapsedTime = NewValues.Key.Value;
	}
	else
	{
		if (bShouldOpen)
		{
			AnimTransforms.Empty();
			
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , ComponentsToMove->GetRelativeLocation()));
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , VectorMoveTo));
			
			if (bDoForever)
			{
				return;
			}
			
			// for (UStaticMeshComponent* Mesh : GetAllStaticMeshesInOwner_Implementation())
			// {
			// 	Mesh->SetCanEverAffectNavigation(false);
			// }
		}
		else
		{
			AnimTransforms.Empty();
			
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , ComponentsToMove->GetRelativeLocation()));
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , FVector::ZeroVector));
			
			// for (UStaticMeshComponent* Mesh : GetAllStaticMeshesInOwner_Implementation())
			// {
			// 	Mesh->SetCanEverAffectNavigation(true);
			// }
		}
	}
	if (bDoForever && 
		bShouldOpen)
	{
		if (IsValid(InteractionToCallInteract.GetObject()))
		{
			InteractionToCallInteract->Interact_Implementation(InteractingCharacter);
		}
		
		return;
	}
	
	bShouldOpen = false;
}

void AInteractionMovingFence::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	InteractingCharacter = InInteractingCharacter;
	
	if (InterwallTimer != 0.0f)
	{
		bDoForever = true;
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &AInteractionMovingFence::ResetDoForever, InterwallTimer, false);
	}
	
	bShouldOpen = true;
	
	if (IsValid(InteractionToCallInteract.GetObject()))
	{
		InteractionToCallInteract->Interact_Implementation(InteractingCharacter);
	}

	const APlayerCamera* PlayerCamera = Cast<APlayerCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass()));
	
	if (!IsValid(PlayerCamera))
	{
		return;
	}

	// UCameraActionStack* CameraActionStack = nullptr;
	//
	// TArray<UCameraActionStack* >Components;
	// PlayerCamera->GetComponents<UCameraActionStack>(Components, true);
	//
	// if (!Components.IsEmpty() && 
	// 	IsValid(Components[0]))
	// {
	// 	CameraActionStack = Components[0];
	// }
	//
	// if (!IsValid(CameraActionStack))
	// {
	// 	return;
	// }
	
	// if (UActionCameraDefaultMovement* ActionRef = CameraActionStack->GetAction<UActionCameraDefaultMovement>())
	// {
	// 	ActionRef->LimitMoveRangeForCamera();
	// }
}

TArray<UStaticMeshComponent*> AInteractionMovingFence::GetAllStaticMeshesInOwner_Implementation()
{
	TArray<UStaticMeshComponent*> FoundComponentsChildren;
	GetComponents<UStaticMeshComponent>(FoundComponentsChildren, true);
	
	return FoundComponentsChildren;
}
