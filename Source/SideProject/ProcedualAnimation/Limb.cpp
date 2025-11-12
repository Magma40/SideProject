// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/Limb.h"

ULimb::ULimb()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULimb::BeginPlay()
{
	Super::BeginPlay();
	AttachedParent = GetAttachParent();
	const TArray<USceneComponent*>& Children = GetAttachChildren();
	AttachedChild = Children.Num() > 0 ? Children[0] : nullptr;
}


void ULimb::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!IsValid(AttachedParent) && !IsValid(AttachedChild)) return;
	
	FVector ParentPos = AttachedParent->GetComponentLocation();
	FVector ChildPos = AttachedChild->GetComponentLocation();

	FVector MidPoint = (ParentPos + ChildPos) * 0.5f;
	FVector Direction = (ParentPos - ChildPos).GetSafeNormal();
	FRotator LookAtRotation = Direction.Rotation();
	LookAtRotation += FRotator(90, 0, 0);
	
	SetWorldLocation(MidPoint);
	SetWorldRotation(LookAtRotation);
}