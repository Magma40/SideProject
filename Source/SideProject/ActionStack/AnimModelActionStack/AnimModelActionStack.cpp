// Fill out your copyright notice in the Description page of Project Settings.
#include "AnimModelActionStack.h"
#include "ProcedualAnimation/MovingModel.h" 
#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"
#include "Tools/GenericCalls.h"
#include "Player/BaseInteractableCharacter.h"
#include "ActionStack/Characters/CompanionActionStack.h"
#include "ActionStack/AnimModelActionStack/LegActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MoveArm.h"

UAnimModelActionStack::UAnimModelActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UAnimModelActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
	Init();
	PushDefaultActionMovement();
}

void UAnimModelActionStack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAnimModelActionStack::Init()
{
	MovingModelRef = Cast<AMovingModel>(GetOwner());
	if (!MovingModelRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - MovingModelRef not found or wrong type"), *StaticClass()->GetName());
		return;
	}
	LegActionStackRef = Cast<ULegActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "LegActionStack"));
	if (!LegActionStackRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - LegActionStackRef not found or wrong type"),  *StaticClass()->GetName());
		return;
	}
	ArmActionStack_LRef = Cast<UArmActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "ArmActionStack_L"));
	if (!ArmActionStack_LRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - ArmActionStack_LRef not found or wrong type"),  *StaticClass()->GetName());
		return;
	}
	ArmActionStack_RRef = Cast<UArmActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "ArmActionStack_R"));
	if (!ArmActionStack_RRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - ArmActionStack_RRef not found or wrong type"),  *StaticClass()->GetName());
		return;
	}
	
	MovingModelRef->InitSceneComponents();
	OwnerCharacter = Cast<ABaseInteractableCharacter>(MovingModelRef->GetParentActor());
	USceneComponent* BackHolsterRef = GenericCalls::FindChildByExactName(GetOwner(), "BackItemHolster");
	if (!BackHolsterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - BackHolsterRef not found or wrong type"), *StaticClass()->GetName());
		return;
	}
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseInteractableCharacter::StaticClass(),  Actors);
	CompanionOwnerCharacter = Cast<ABaseInteractableCharacter>(Actors[0]);
	if (CompanionOwnerCharacter == OwnerCharacter) CompanionOwnerCharacter = Cast<ABaseInteractableCharacter>(Actors[1]);

	LegActionStackRef->Init(MovingModelRef->GetLegs(), OwnerCharacter);
	TArray<UMoveArm*>  Arms =  MovingModelRef->GetArms();
	if (Arms[0]->GetName().EndsWith(TEXT("_L"))) // This is kinda hard coding ):
	{
		ArmActionStack_LRef->Init(Arms[0], OwnerCharacter, CompanionOwnerCharacter, BackHolsterRef);
		ArmActionStack_RRef->Init(Arms[1], OwnerCharacter, CompanionOwnerCharacter, BackHolsterRef);
	}
	else
	{
		ArmActionStack_RRef->Init(Arms[0], OwnerCharacter, CompanionOwnerCharacter, BackHolsterRef);
		ArmActionStack_LRef->Init(Arms[1], OwnerCharacter, CompanionOwnerCharacter, BackHolsterRef);
	}
	MovingModelRef->SetAnimModelActionStack(this);
}

void UAnimModelActionStack::PushDefaultActionMovement()
{
	UActionAnimModelMovingMovement* DefaultMovement = UActionAnimModelMovingMovement::Create(this, MovingModelRef);
	PushAction(DefaultMovement);

	LegActionStackRef->AddMovingAction();
	ArmActionStack_RRef->AddMovingAction();
	ArmActionStack_LRef->AddMovingAction();
}

void UAnimModelActionStack::PushJumpingActionMovement()
{
	LegActionStackRef->AddJumpingAction();
	if (OwnerCharacter->GetCompanionActionStack()) return;
	
	TScriptInterface<IActionInterface> ActionInterfaceHandingHolding = GetInterfacedAction(ArmActionStack_LRef->GetAction<UActionArmHandHolding>());
	if (!ActionInterfaceHandingHolding) return;
	
	if (ArmActionStack_LRef->Contains(ActionInterfaceHandingHolding))
	{
		CompanionOwnerCharacter->Jump() ;
	}
}

void UAnimModelActionStack::PushHolsterActionMovement() const
{
	ArmActionStack_RRef->AddHolsterAction();
}

void UAnimModelActionStack::PushWaveActionMovement() const
{
	ArmActionStack_RRef->AddWaveAction();
}

void UAnimModelActionStack::PushPointingActionMovement() const
{
	ArmActionStack_RRef->AddPointingAction();
}

void UAnimModelActionStack::PushHandHoldingActionMovement()
{
	if (OwnerCharacter->GetCompanionActionStack()) return;
	if (CompanionOwnerCharacter->GetItemInHand() != nullptr) return;

	float HandDistances = FVector::Distance(OwnerCharacter->GetMovingModel()->GetLeftHand()->GetComponentLocation(), CompanionOwnerCharacter->GetMovingModel()->GetRightHand()->GetComponentLocation());
	if (HandDistances > MinDistanceToHoldHands) return;

	TScriptInterface<IActionInterface> ActionInterfaceContinuousMoveTo = GetInterfacedAction(CompanionOwnerCharacter->GetCompanionActionStack()->GetAction<UActionContinuousMoveTo>());
	if (!CompanionOwnerCharacter->GetCompanionActionStack()->Contains(ActionInterfaceContinuousMoveTo))
	{
		ArmActionStack_LRef->AddHandHoldingAction(CompanionOwnerCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->GetArm());
		CompanionOwnerCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->AddHandHoldingAction(ArmActionStack_LRef->GetArm());
		CompanionOwnerCharacter->GetCompanionActionStack()->OnAddActionContinuousMove(OwnerCharacter);
	}
	else
	{
		TScriptInterface<IActionInterface> CompanionHandHoldingAction = GetInterfacedAction(CompanionOwnerCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->GetAction<UActionArmHandHolding>());
		TScriptInterface<IActionInterface> PlayerHandHoldingAction = GetInterfacedAction(ArmActionStack_LRef->GetAction<UActionArmHandHolding>());
		ArmActionStack_LRef->RemoveAction(PlayerHandHoldingAction);
		CompanionOwnerCharacter->GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_R()->RemoveAction(CompanionHandHoldingAction);
		CompanionOwnerCharacter->GetCompanionActionStack()->RemoveAction(ActionInterfaceContinuousMoveTo);
	}
}
