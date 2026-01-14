// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/Limb.h"
#include "Tools/GenericCalls.h"

ULimb::ULimb() { PrimaryComponentTick.bCanEverTick = true; }

void ULimb::BeginPlay()
{
	Super::BeginPlay();
	TArray<USceneComponent*> AllComponents;
	GetOwner()->GetComponents<USceneComponent>(AllComponents);
	for (USceneComponent* Comp : AllComponents)
	{
		if (!Comp) continue;
		if (Comp->GetName() == AttachedParentName) AttachedParent = Comp;
		if (Comp->GetName() == AttachedChildName) AttachedChild =  Comp;
	}
}

void ULimb::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!IsValid(AttachedParent) && !IsValid(AttachedChild)) return;;
	const FVector ParentPos = AttachedParent->GetComponentLocation();
	const FVector ChildPos = AttachedChild->GetComponentLocation();

	const FVector MidPoint = (ParentPos + ChildPos) * 0.5f;
	const FVector Direction = (ParentPos - ChildPos).GetSafeNormal();
	FRotator LookAtRotation = Direction.Rotation();
	LookAtRotation += FRotator(90, 0, 0);
	
	SetWorldLocation(MidPoint);
	SetWorldRotation(LookAtRotation);
}