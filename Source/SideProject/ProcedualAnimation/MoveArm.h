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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIDEPROJECT_API UMoveArm : public USceneComponent
{
	GENERATED_BODY()
	
	UMoveArm();

	//FUNCTIONS:
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	void Init();
	
	void UpdateIK();

	//FIELDS:
public:
	UPROPERTY()
	FArmsPose TargetPose = FArmsPose();
	
	UPROPERTY()
	TArray<TObjectPtr<USceneComponent>> ArmParts;
	
	bool bEnableArmConstraints = true;
};

static FVector ConstrainSwingTwist( const FVector& RestDirWS, 
	const FVector& DesiredDirWS,
	const float MaxSwingDegrees)
{
	//Quaternion rotating rest direction → desired direction
	const FQuat Delta = FQuat::FindBetweenNormals(RestDirWS, DesiredDirWS);

	//Decompose into swing + twist around RestDir
	FQuat Swing, Twist;
	Delta.ToSwingTwist(RestDirWS, Swing, Twist);

	//Extract swing angle
	FVector SwingAxis;
	float SwingAngle;
	Swing.ToAxisAndAngle(SwingAxis, SwingAngle);

	//Clamp swing angle
	const float MaxSwingRad = FMath::DegreesToRadians(MaxSwingDegrees);
	SwingAngle = FMath::Clamp(SwingAngle, -MaxSwingRad, MaxSwingRad);

	//Rebuild constrained swing
	Swing = FQuat(SwingAxis, SwingAngle);

	//Combine swing + original twist
	const FQuat Limited = Swing * Twist;

	//Apply to rest direction
	return Limited.RotateVector(RestDirWS).GetSafeNormal();
}
