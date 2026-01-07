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
#include "GameFramework/CharacterMovementComponent.h"


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
	ArmActionStack_LRef = Cast<UArmActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "ArmActionStack_L"));
	if (!ArmActionStack_LRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - ArmActionStack_LRef not found or wrong type"), *GetOwner()->GetName());
		return;
	}
	ArmActionStack_RRef = Cast<UArmActionStack>(GenericCalls::FindChildByExactName(GetOwner(), "ArmActionStack_R"));
	if (!ArmActionStack_RRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:BeginPlay - ArmActionStack_RRef not found or wrong type"), *GetOwner()->GetName());
		return;
	}
	
	Init();
	PushDefaultActionMovement();
}

void UAnimModelActionStack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Cast<ACompanionPawn>(OwnerCharacter)) return;

	TScriptInterface<IActionInterface> ActionInterfaceHandingHolding = GetInterfacedAction(ArmActionStack_LRef->GetHandHoldingAction());
	if (!ActionInterfaceHandingHolding) return;

	if (ArmActionStack_LRef->Contains(ActionInterfaceHandingHolding))
	{
		if (OwnerCharacter->Sprinting) CompanionOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = CompanionOwnerCharacter->MaxSprintSpeed;
		else  CompanionOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = CompanionOwnerCharacter->WalkSpeed;
		
		ACompanionPawn* AICompanion =  Cast<ACompanionPawn>(CompanionOwnerCharacter);
		TScriptInterface<IActionInterface> ActionInterfaceMoveTo = GetInterfacedAction(AICompanion->CompanionData->GetMoveTo());
		if (!CompanionOwnerCharacter->MovingModel->AnimModelActionStack->GetArmActionStack_R()->Contains(ActionInterfaceHandingHolding) || !ActionInterfaceMoveTo)
		{
			AICompanion->AddNewMoveTo(OwnerCharacter->GetActorLocation());
		}
	}
}

void UAnimModelActionStack::Init()
{
	MovingModelRef->InitSceneComponents();
	OwnerCharacter = Cast<ABaseInteractableCharacter>(MovingModelRef->GetParentActor());
	USceneComponent* BackHolsterRef = GenericCalls::FindChildByExactName(GetOwner(), "BackItemHolster");
	if (!BackHolsterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:Init - BackHolsterRef not found or wrong type"), *GetOwner()->GetName());
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
	MovingModelRef->AnimModelActionStack = this;
}

void UAnimModelActionStack::PushDefaultActionMovement()
{
	DefaultMovement = NewObject<UActionAnimModelMovingMovement>(this);
	DefaultMovement->Init(MovingModelRef);
	PushAction(DefaultMovement);

	LegActionStackRef->AddMovingAction();
	ArmActionStack_RRef->AddMovingAction();
	ArmActionStack_LRef->AddMovingAction();
}

void UAnimModelActionStack::PushJumpingActionMovement()
{
	LegActionStackRef->AddJumpingAction();
	if (Cast<ACompanionPawn>(OwnerCharacter)) return;
	
	TScriptInterface<IActionInterface> ActionInterfaceHandingHolding = GetInterfacedAction(ArmActionStack_LRef->GetHandHoldingAction());
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

void UAnimModelActionStack::PushHandHoldingActionMovement() const
{
	if (Cast<ACompanionPawn>(OwnerCharacter)) return;
	if (CompanionOwnerCharacter->ItemInHand != nullptr) return;

	float HandDistances = FVector::Distance(OwnerCharacter->MovingModel->GetLeftHand()->GetComponentLocation(), CompanionOwnerCharacter->MovingModel->GetRightHand()->GetComponentLocation());
	if (HandDistances > MinDistanceToHoldHands) return;
	
	ArmActionStack_LRef->AddHandHoldingAction(CompanionOwnerCharacter->MovingModel->AnimModelActionStack->GetArmActionStack_R()->GetArm());
	CompanionOwnerCharacter->MovingModel->AnimModelActionStack->GetArmActionStack_R()->AddHandHoldingAction(ArmActionStack_LRef->GetArm());
}
