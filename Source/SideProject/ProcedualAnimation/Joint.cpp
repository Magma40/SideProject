// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/Joint.h"

UJoint::UJoint() { PrimaryComponentTick.bCanEverTick = true; }

void UJoint::BeginPlay()
{
	Super::BeginPlay();
	TArray<USceneComponent*> AllComponents;
	GetOwner()->GetComponents<USceneComponent>(AllComponents);
	for (USceneComponent* Comp : AllComponents)
	{
		if (!Comp) continue;
		if (Comp->GetName() == JointName) JointReference = Comp;
	}
}

void UJoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!IsValid(JointReference)) return;;
	SetWorldLocation(JointReference->GetComponentLocation());
	SetWorldRotation(JointReference->GetComponentRotation());
}