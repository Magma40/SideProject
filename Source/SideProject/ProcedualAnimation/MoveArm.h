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

protected: //Functions
	UMoveArm();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init();

public:
	void UpdateIK();
	bool ShouldHoldUpHands();

public: // Variables
	FArmsPose TargetPose = FArmsPose();
	TArray<USceneComponent*> ArmParts;
	TArray<UMoveArm*> OtherArms;
	bool bEnableArmConstraints = true;
};

static FVector ConstrainSwingTwist( const FVector& RestDirWS, const FVector& DesiredDirWS, float MaxSwingDegrees)
{
	// Quaternion rotating rest direction → desired direction
	FQuat Delta = FQuat::FindBetweenNormals(RestDirWS, DesiredDirWS);

	// Decompose into swing + twist around RestDir
	FQuat Swing, Twist;
	Delta.ToSwingTwist(RestDirWS, Swing, Twist);

	// Extract swing angle
	FVector SwingAxis;
	float SwingAngle;
	Swing.ToAxisAndAngle(SwingAxis, SwingAngle);

	// Clamp swing angle
	float MaxSwingRad = FMath::DegreesToRadians(MaxSwingDegrees);
	SwingAngle = FMath::Clamp(SwingAngle, -MaxSwingRad, MaxSwingRad);

	// Rebuild constrained swing
	Swing = FQuat(SwingAxis, SwingAngle);

	// Combine swing + original twist
	FQuat Limited = Swing * Twist;

	// Apply to rest direction
	return Limited.RotateVector(RestDirWS).GetSafeNormal();
}
