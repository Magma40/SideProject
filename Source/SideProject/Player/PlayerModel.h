// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerModel.generated.h"

UCLASS()
class SIDEPROJECT_API APlayerModel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerModel();

	UStaticMeshComponent* Hips = nullptr;
	UStaticMeshComponent* Neck = nullptr;
	UStaticMeshComponent* Waist= nullptr;

	//Legs 
	UStaticMeshComponent* Leg_R = nullptr;
	UStaticMeshComponent* LegJoint_R = nullptr;
	UStaticMeshComponent* FootJoint_R = nullptr;
	
	UStaticMeshComponent* Leg_L = nullptr;
	UStaticMeshComponent* LegJoint_L = nullptr;
	UStaticMeshComponent* FootJoint_L = nullptr;

	//Arms
	UStaticMeshComponent* Arm_R = nullptr;
	UStaticMeshComponent* ArmJoint_R = nullptr;
	UStaticMeshComponent* HandJoint_R = nullptr;
	
	UStaticMeshComponent* Arm_L = nullptr;
	UStaticMeshComponent* ArmJoint_L = nullptr;
	UStaticMeshComponent* HandJoint_L = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UStaticMeshComponent* FindChild(UStaticMeshComponent* Parent, FName Name);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
