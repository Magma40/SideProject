#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmPoint.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "ProcedualAnimation/MoveArm.h"
#include "Components/StaticMeshComponent.h"

void UActionArmPoint::OnUpdate(float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MoveArmRef));
	check(!MoveArmRef->ArmParts.IsEmpty())
	check(IsValid(AnimSubsystem));

	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	FQuat RelativeQuat;

	FVector LocationA = MoveArmRef->ArmParts[1]->GetComponentLocation();
	FVector LocationB = MoveArmRef->ArmParts[2]->GetComponentLocation();

	//Calculate which direction the arm should point at
	FVector Direction = LocationB - LocationA;
	Direction.Normalize();
	
	FVector NewZAxis = Direction;
	FVector WorldUpVector = FVector::UpVector; 
	
	FVector NewXAxis = FVector::CrossProduct(WorldUpVector, NewZAxis).GetSafeNormal();
	
	if (NewXAxis.IsNearlyZero())
	{
		FVector WorldRightVector = FVector::RightVector;
		NewXAxis = FVector::CrossProduct(WorldRightVector, NewZAxis).GetSafeNormal();
	}

	//Make it so the "top" of hand is facing the direction of the pointing location
	FVector NewYAxis = FVector::CrossProduct(NewZAxis, NewXAxis).GetSafeNormal();
	FMatrix Matrix(FPlane(NewXAxis, 0.f), 
		FPlane(NewYAxis, 0.f), 
		FPlane(NewZAxis, 0.f), 
		FPlane(0.f, 0.f, 0.f, 1.f));
	
	FRotator NewRotation = Matrix.Rotator();
	RelativeQuat = Xform.InverseTransformRotation(NewRotation.Quaternion());

	//Construct animation points to point arm at pointing location
	AnimTransforms.Empty();
	
	FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	AnimTransforms.Add(FTransform(RelativeQuat, HandLocationPos));

	FVector PointLocation = Xform.InverseTransformPosition(PointingAtLocation);
	AnimTransforms.Add(FTransform(RelativeQuat, PointLocation));

	//Update arm pointing animation 
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms, 
			ElapsedTime, 
			PlayRate);
	
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();
}