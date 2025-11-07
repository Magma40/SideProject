// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerModel.h"

// Sets default values
APlayerModel::APlayerModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void APlayerModel::BeginPlay()
{
	Super::BeginPlay();
	Hips = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Leg_L"));
	Waist = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Waist"));
	Neck = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Neck"));

	Leg_L = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Leg_L"));
	LegJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("LegJoint_L"));
	FootJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("FootJoint_L"));
	
	Leg_R = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Leg_R"));
	LegJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("LegJoint_R"));
	FootJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("FootJoint_R"));
	
	Arm_L = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Arm_L"));
	ArmJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("ArmJoint_L"));
	HandJoint_L = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("HandJoint_L"));

	Arm_R = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("Arm_R"));
	ArmJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("ArmJoint_R"));
	HandJoint_R = FindChild<UStaticMeshComponent>(GetRootComponent(), FName("HandJoint_R"));
}

// Called every frame
void APlayerModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

