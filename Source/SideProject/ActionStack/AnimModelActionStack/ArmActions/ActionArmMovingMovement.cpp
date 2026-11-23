#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmMovingMovement.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MoveArm.h"

void UActionArmMovingMovement::OnBegin(const bool bFirstTime)
{
	Super::OnBegin(bFirstTime);
	
	check(IsValid(MoveArmRef));
	
	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
}

void UActionArmMovingMovement::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MoveArmRef));
	
	MoveArmRef->TargetPose.Position = FVector(0,25,  -50);
	MoveArmRef->TargetPose.Rotation = FRotator(0, 0, 0).Quaternion();
}

bool UActionArmMovingMovement::ShouldHoldUpHands() const
{
	check(IsValid(MoveArmRef));
	check(!MoveArmRef->ArmParts.IsEmpty())
	
	MoveArmRef->bEnableArmConstraints = false;
	
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector StartWorld = Xform.GetLocation();
	const FVector EndWorld   = Xform.GetLocation() + MoveArmRef->ArmParts[0]->GetRightVector() * 150;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MoveArmRef->ArmParts[0]->GetOwner()->GetParentActor());
	Params.AddIgnoredActor(MoveArmRef->ArmParts[0]->GetOwner());
	TArray<AActor*> ActorsAround;
	
	UGameplayStatics::GetAllActorsWithTag (GetWorld(), "HandHeldItem", ActorsAround);
	
	for (const AActor* Actor : ActorsAround)
	{
		Params.AddIgnoredActor(Actor);
	}
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartWorld, EndWorld, ECC_Visibility, Params))
	{
		const FVector LocalPoint = Xform.InverseTransformPosition(Hit.ImpactPoint);
		
		MoveArmRef->TargetPose.Position = LocalPoint;
		MoveArmRef->TargetPose.Rotation = 	FQuat::FindBetweenNormals( Xform.InverseTransformVector(FVector::UpVector), Xform.InverseTransformVector(Hit.ImpactNormal).GetSafeNormal());
		
		return true;
	}
	
	return false;
}