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
	APlayerModel();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	// === Movement Settings ===
	UPROPERTY(EditAnywhere, Category = "Hips")
	float MoveSpeed = 800.0f; 

	UPROPERTY(EditAnywhere, Category = "Hips")
	float RotationSpeed = 1.0f; 

	UPROPERTY(EditAnywhere, Category = "Hips")
	float HipSwayAmount = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Hips")
	float HipSmoothSpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Hips")
	USceneComponent* Hip = nullptr;

	UPROPERTY(EditAnywhere, Category = "Hips")
	AActor* MoveTarget = nullptr;

	UPROPERTY(EditAnywhere, Category="Hips")
	float HipsApplyHeight = 0.0f;

protected:
	TArray<ULeg*> Legs;

	float HipBaseHeight = 0.f;
	
public:	
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