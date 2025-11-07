// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerModel.h"

// Sets default values
APlayerModel::APlayerModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

UStaticMeshComponent*APlayerModel::FindChild(UStaticMeshComponent* Parent,  FName Name)
{
	TArray<USceneComponent*> Childs;
	for (UStaticMeshComponent* Component : Parent->GetChildrenComponents(true, Childs))
	{
		if (IsValid(Component) && Component->GetName() == Name.ToString())
		{
			// Found the component
			return Component;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("PlayerModel::FindChild - Could Not Find The Component"));
	return nullptr;
}

// Called when the game starts or when spawned
void APlayerModel::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

