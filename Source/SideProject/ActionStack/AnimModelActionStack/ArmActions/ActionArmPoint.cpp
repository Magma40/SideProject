// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmPoint.h"

#include "Camera/CameraActor.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Player/BaseInteractableCharacter.h"

UActionArmPoint::UActionArmPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void UActionArmPoint::Init(UMoveArm* InMoveArm, ABaseInteractableCharacter* InCharacter)
{
	MoveArmRef = InMoveArm;
	OwnerCharacter = InCharacter;
}

void UActionArmPoint::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimLocations.Insert(HandLocationPos, 0);
	
	FHitResult Hit;
	FVector Start = OwnerCharacter->TheMainCameraInScene->GetActorLocation();
	FVector End = Start + (OwnerCharacter->TheMainCameraInScene->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		PointingLocation = Hit.ImpactPoint;
		FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
		AnimLocations.Insert(PointLocation, 1);
	}
}

void UActionArmPoint::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	if (AnimLocations.Num() > 1)
	{
		UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
		if (!AnimSubsystem)
		{
			UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
			return;
		}
		const TTuple<TTuple<TArray<FVector>, float>,FVector> NewValues = AnimSubsystem->CustomAnim(DeltaTime, AnimLocations, ElapsedTime, PlayRate);
		MoveArmRef->TargetPose.Position = NewValues.Value;
		AnimLocations = NewValues.Key.Key;
		ElapsedTime = NewValues.Key.Value;
		return;
	}

	FHitResult Hit;
	FVector Start = OwnerCharacter->TheMainCameraInScene->GetActorLocation();
	FVector End = Start + (OwnerCharacter->TheMainCameraInScene->GetActorForwardVector() * 10000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params ))
	{
		if (Hit.GetComponent()->GetOwner()->ActorHasTag("HandHeldItem"))
		{
			ActorPointedAt = Hit.GetComponent();
		}
		else ActorPointedAt = nullptr;
		
		PointingLocation = Hit.ImpactPoint;
		const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
		FVector PointLocation = Xform.InverseTransformPosition(Hit.ImpactPoint);
		MoveArmRef->TargetPose.Position = PointLocation;
		MoveArmRef->TargetPose.Rotation = FQuat::Identity;
		DrawDebugLine(GetWorld(),Start + (OwnerCharacter->TheMainCameraInScene->GetActorForwardVector() * 100.f),Hit.Location,FColor(255, 0, 0),false, -1, 0,1);
	}
	else
	{
		ActorPointedAt = nullptr;
		const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
		FVector PointLocation = Xform.InverseTransformPosition(End);
		MoveArmRef->TargetPose.Position = PointLocation;
		MoveArmRef->TargetPose.Rotation = FQuat::Identity;
	}
}

bool UActionArmPoint::IsDone_Implementation() const
{
	return false;
}
