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
	//
	// FVector ParentPos = AttachedParent->GetComponentLocation();
	// FVector ChildPos = AttachedChild->GetComponentLocation();
	//
	// FVector Direction = (ParentPos - ChildPos).GetSafeNormal();
	// FRotator LookAtRotation = Direction.Rotation();
	// FRotator FinalRotation = LookAtRotation + MeshOrientationOffset;
	//
	// FVector SizeDirection = (ParentPos - ChildPos);
	// LookAtRotation += FRotator(90, 0, 0);
	// ParentPos += GetUpVector() * SizeDirection.Length() / 2.f;
	// this->SetWorldLocation(ParentPos);
	// this->SetWorldRotation(LookAtRotation);
	//  LookAtRotation = FRotator(0, 0, 0);
	// AttachedChild->SetWorldRotation(LookAtRotation);

	// float LegLength = FVector::Distance(ParentPos, ChildPos);
	// FVector CurrentScale = this->GetComponentScale();
	// this->SetWorldScale3D(FVector(LegLength / 100.f, CurrentScale.Y, CurrentScale.Z));
	//UE_LOG(LogTemp, Warning, TEXT("%s An Actor's name is %s"), *this->GetName(), *this->GetComponentRotation().ToString());
}