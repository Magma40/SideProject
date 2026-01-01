// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmToHolster.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"

UActionArmToHolster::UActionArmToHolster(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer) {}

void UActionArmToHolster::Init(UMoveArm* InMoveArm)
{
	MoveArmRef = InMoveArm;
}

void UActionArmToHolster::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionArmToHolster::OnUpdate_Implementation(float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();;
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UMoveArm - SetWaveAnim - AnimSubsystem was nullptr"));
		return;
	}
	AnimSubsystem->PlayCurveAnim(HolsterAnimCurve, 2.0f, [&](float CurrValue)
	{
		MoveArmRef->TargetPose.Position = FVector(CurrValue * 25, 0, CurrValue * 50);
	});
}

bool UActionArmToHolster::IsDone_Implementation() const
{
	return false;
}
