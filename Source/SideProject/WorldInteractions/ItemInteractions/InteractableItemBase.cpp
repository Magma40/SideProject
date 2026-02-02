// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldInteractions/ItemInteractions/InteractableItemBase.h"

AInteractableItemBase::AInteractableItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AInteractableItemBase::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (MaterialOverlayStatus)
	{
		case EMaterialOverlayStatus::None: break;;
		case EMaterialOverlayStatus::Active: { EnableMaterialInterface_Implementation(); break; }
		case EMaterialOverlayStatus::Inactive: { DisableMaterialInterface_Implementation(); break; }
	}
}

void AInteractableItemBase::AttachToComponent_Implementation(USceneComponent* InAttachToComponent)
{
	for (UPrimitiveComponent* Prim : GetPrimitiveComponentChildren_Implementation())
	{
		if (!IsValid(Prim)) continue;
		Prim->SetSimulatePhysics(false);
		Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	DisableMaterialInterface_Implementation();
	GetRootComponent()->AttachToComponent(InAttachToComponent ,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AInteractableItemBase::DetachFromComponent_Implementation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	for (UPrimitiveComponent* Prim : GetPrimitiveComponentChildren_Implementation())
	{
		if (!IsValid(Prim)) continue;
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	DisableMaterialInterface_Implementation();
}

void AInteractableItemBase::EnableMaterialInterface_Implementation()
{
	const TArray<UStaticMeshComponent*> StaticMeshes = GetStaticMeshChildren_Implementation();
	if (StaticMeshes.IsEmpty()) return;
	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshes)
	{
		if (IsValid(StaticMeshComponent))
		{
			StaticMeshComponent->SetOverlayMaterial(OutlineMaterialInterface);
		}
	}
	MaterialOverlayStatus = EMaterialOverlayStatus::Inactive;
}

void AInteractableItemBase::DisableMaterialInterface_Implementation()
{
	const TArray<UStaticMeshComponent*> StaticMeshes = GetStaticMeshChildren_Implementation();
	if (StaticMeshes.IsEmpty()) return;
	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshes)
	{
		if (IsValid(StaticMeshComponent))
		{
			StaticMeshComponent->SetOverlayMaterial(nullptr);
		}
	}
	MaterialOverlayStatus = EMaterialOverlayStatus::None;
}

TArray<UPrimitiveComponent*>  AInteractableItemBase::GetPrimitiveComponentChildren_Implementation()
{
	TArray<UPrimitiveComponent*> Primitives;
	GetComponents<UPrimitiveComponent>(Primitives);
	return Primitives;
}

TArray<UStaticMeshComponent*> AInteractableItemBase::GetStaticMeshChildren_Implementation()
{
	TArray<UStaticMeshComponent*> StaticMeshes;
	GetComponents<UStaticMeshComponent>(StaticMeshes);
	return StaticMeshes;
 }


