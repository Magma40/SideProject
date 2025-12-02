// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionStack/Characters/ActionCharacter.h"

void UActionCharacter::OnBegin_Implementation(bool bFirstTime) 
{
	Super::OnBegin_Implementation(bFirstTime);
	//if (bFirstTime)
}
bool UActionCharacter::IsDone_Implementation() const
{
	return true;
}

