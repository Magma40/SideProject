// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PropertyCustomizationHelpers.h"
#include "GameFramework/Actor.h"
#include "PlayerModel.generated.h"

class ULeg;

UCLASS()
class SIDEPROJECT_API APlayerModel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerModel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HipSwayAmount = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HipsInModel = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FTransform MoveTarget;

private:
	float HipHeight = 1.0f;

#pragma region BodyParts

	UStaticMeshComponent* Neck = nullptr;
	UStaticMeshComponent* Waist= nullptr;
	TArray<ULeg*> Legs;

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
#pragma endregion BodyParts
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	template <typename T>
		T* FindChild(USceneComponent* Parent,  FName Name)
	{
		if (!Parent) return nullptr;
		for (USceneComponent* Component : Parent->GetAttachChildren())
		{
			if (!Component) continue;
			if (Component->GetFName() == Name)
			{
				if (T* TypedComponent = Cast<T>(Component))
				{
					return TypedComponent;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Found component named %s, but it’s not of type %s"),
						*Name.ToString(), *T::StaticClass()->GetName());
				}
			}
			if (T* Found = FindChild<T>(Component, Name))
			{
				return Found;
			}
		}
		if (Parent == GetRootComponent())
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerModel::FindChild - Could not find component with name: %s"), *Name.ToString());
		}
		return nullptr;
	}	
};