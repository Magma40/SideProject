// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmMovingMovement.h"

#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MoveArm.h"

UActionArmMovingMovement* UActionArmMovingMovement::Create(UObject* Outer, UMoveArm* InMoveArm)
{
	UActionArmMovingMovement* Action = NewObject<UActionArmMovingMovement>(Outer);
	Action->MoveArmRef = InMoveArm;
	return Action;
}

void UActionArmMovingMovement::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
}

void UActionArmMovingMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
	// if (ShouldHoldUpHands()) {}
	// else
	// {
	// 	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	// 	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
	// }
}

bool UActionArmMovingMovement::IsDone_Implementation() const
{
	return false;
}

bool UActionArmMovingMovement::ShouldHoldUpHands() const
{
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FVector StartWorld = Xform.GetLocation();
	FVector EndWorld   = Xform.GetLocation() + MoveArmRef->ArmParts[0]->GetRightVector() * 75;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MoveArmRef->ArmParts[0]->GetOwner()->GetParentActor());
	Params.AddIgnoredActor(MoveArmRef->ArmParts[0]->GetOwner());
	TArray<AActor*> ActorsAround; //  Gather actors
	UGameplayStatics::GetAllActorsWithTag (GetWorld(), "HandHeldItem", ActorsAround);
	for (AActor* Actor : ActorsAround)
	{
		Params.AddIgnoredActor(Actor);
	}
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		FVector LocalPoint = Xform.InverseTransformPosition(Hit.ImpactPoint);
		MoveArmRef->TargetPose.Position = LocalPoint;
		MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();

		FVector UpLocal = Xform.InverseTransformVector(FVector::UpVector);
		FVector NormalLocal = Xform.InverseTransformVector(Hit.ImpactNormal);
		//TargetPose.Rotation = FQuat::FindBetweenNormals(UpLocal, NormalLocal);
		return true;
	}
	return false;
}
