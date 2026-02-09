#include "InteractionMovingFence.h"

#include "Core/Subsystems/AnimationCurveSubsystem.h"

AInteractionMovingFence::AInteractionMovingFence() { PrimaryActorTick.bCanEverTick = true;  }

void AInteractionMovingFence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	USceneComponent* ComponentsToMove = Cast<USceneComponent>(ComponentsToMoveRef.GetComponent(this));
	if (!IsValid(ComponentsToMove)) return;

	if (AnimTransforms.Num() > 1)
	{
		const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
		if (!AnimSubsystem)
		{
			UE_LOG(LogTemp, Error,TEXT("%s - Tick - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
			return;
		}
		const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimTransforms, ElapsedTime, PlayRate);
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
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , FVector::ZeroVector + FVector(0.0f, 0.0f, ZMoveTo)));
			for (UStaticMeshComponent* Mesh : GetAllStaticMeshesInOwner_Implementation())
			{
				Mesh->SetCanEverAffectNavigation(false);
			}
		}
		else
		{
			AnimTransforms.Empty();
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , ComponentsToMove->GetRelativeLocation()));
			AnimTransforms.Add(FTransform(ComponentsToMove->GetComponentRotation().Quaternion() , FVector::ZeroVector));
			for (UStaticMeshComponent* Mesh : GetAllStaticMeshesInOwner_Implementation())
			{
				Mesh->SetCanEverAffectNavigation(true);
			}
		}
	}
	bShouldOpen = false;
}

void AInteractionMovingFence::Interact_Implementation(ABaseInteractableCharacter* InInteractingCharacter)
{
	bShouldOpen = true;
}

TArray<UStaticMeshComponent*> AInteractionMovingFence::GetAllStaticMeshesInOwner_Implementation()
{
	TArray<UStaticMeshComponent*> FoundComponentsChildren;
	GetComponents<UStaticMeshComponent>(FoundComponentsChildren, true);
	return FoundComponentsChildren;
}
