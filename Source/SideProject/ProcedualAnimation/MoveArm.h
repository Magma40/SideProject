// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MoveArm.generated.h"


USTRUCT(BlueprintType)
struct FArmsPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation = FQuat::Identity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEPROJECT_API UMoveArm : public USceneComponent
{
	GENERATED_BODY()

protected:	
	UMoveArm();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init();
	FArmsPose ShouldHoldUpHands() const;

public:
	void UpdateArm();

private:
	TArray<USceneComponent*> ArmParts;
	TArray<UMoveArm*> OtherArms;
	FArmsPose CurrentArmPose;
};
