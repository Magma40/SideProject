// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimModelActionStack.h"
#include "ProcedualAnimation/MovingModel.h" // <- the header that declares UMovingModel
#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"
#include "Tools/GenericCalls.h"
#include "Player/BaseInteractableCharacter.h"

#include "ActionStack/AnimModelActionStack/LegActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/Characters/CompanionPawn.h"
#include "Kismet/GameplayStatics.h"

UAnimModelActionStack::UAnimModelActionStack() { PrimaryComponentTick.bCanEverTick = true; }

void UAnimModelActionStack::BeginPlay()
{
	Super::BeginPlay();
	Main = this;
	MovingModelRef = Cast<AMovingModel>(GetOwner());
	if (!MovingModelRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - MovingModelRef not found or wrong type"), *GetOwner()->GetName());
		return;
	}
	LegActionStackRef = Cast<ULegActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "LegActionStack"));
	if (!LegActionStackRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - LegActionStackRef not found or wrong type"), *GetOwner()->GetName());
		return;
	}
	ArmActionStackRef = Cast<UArmActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "ArmActionStack"));
	if (!ArmActionStackRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - ArmActionStackRef not found or wrong type"), *GetOwner()->GetName());
		return;
	}
	Init();
	PushDefaultActionMovement();
}

void UAnimModelActionStack::Init()
{
	MovingModelRef->InitSceneComponents();
	OwnerCharacter = Cast<ABaseInteractableCharacter>(MovingModelRef->GetParentActor());
	LegActionStackRef->Init(MovingModelRef->GetLegs(), OwnerCharacter);
	USceneComponent* BackHolsterRef = GenericCalls::FindChildByExactName(GetOwner(), "BackItemHolster");
	if (!BackHolsterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - BackHolsterRef not found or wrong type"), *GetOwner()->GetName());
		return;
	}
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACompanionPawn::StaticClass(),  Actors);
	AICompanion = Cast<ACompanionPawn>(Actors[0]);
	
	ArmActionStackRef->Init(MovingModelRef->GetArms(), OwnerCharacter, BackHolsterRef, AICompanion);
	MovingModelRef->AnimModelActionStack = this;
}

void UAnimModelActionStack::PushDefaultActionMovement()
{
	DefaultMovement = NewObject<UActionAnimModelMovingMovement>(this);
	DefaultMovement->Init(MovingModelRef);
	PushAction(DefaultMovement);

	LegActionStackRef->AddMovingAction();
	ArmActionStackRef->AddMovingAction();
}

void UAnimModelActionStack::PushJumpingActionMovement() const
{
	LegActionStackRef->AddJumpingAction();
}

void UAnimModelActionStack::PushHolsterActionMovement() const
{
	ArmActionStackRef->AddHolsterAction();
}

void UAnimModelActionStack::PushWaveActionMovement() const
{
	ArmActionStackRef->AddWaveAction();
}

void UAnimModelActionStack::PushPointingActionMovement() const
{
	ArmActionStackRef->AddPointingAction();
}
