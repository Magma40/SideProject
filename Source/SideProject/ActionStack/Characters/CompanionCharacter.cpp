// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/CompanionCharacter.h"
#include "ActionStack/Actions/ActionMoveTo.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MainCharacter.h"

void UCompanionCharacter::Initialize(APawn* InActorOwner)
{
	ActorOwner = InActorOwner;
}
void UCompanionCharacter::OnBegin_Implementation(bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	if (!bFirstTime || !ActorOwner) return;
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));
	if (!IsValid(MainCharacter)) return;
	
	UActionMoveTo* MoveAction = NewObject<UActionMoveTo>(this);
	MoveAction->Init(this, MainCharacter->GetActionLocation());
	UActionStack::GetMain()->PushAction(MoveAction);
}
