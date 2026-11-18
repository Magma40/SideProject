#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandToLocation.h"
#include "Core/Subsystems/AnimationCurveSubsystem.h"
#include "Player/BaseInteractableCharacter.h"
#include "Player/CompanionPawn.h"
#include "ProcedualAnimation/MoveArm.h"
#include "WorldObjects/CompanionUIIndicator.h"

UActionArmHandToLocation* UActionArmHandToLocation::Create(UObject* Outer, 
	ABaseInteractableCharacter* InOwnerCharacter, 
	UMoveArm* InMoveArm, 
	const FVector& InToLocation,
	const bool bWantToPickUpItem)
{
	UActionArmHandToLocation* Action = NewObject<UActionArmHandToLocation>(Outer);
	
	Action->OwnerCharacter = InOwnerCharacter;
	Action->MoveArmRef = InMoveArm;
	Action->ToLocation = InToLocation;
	Action->bWantToPickUpItem = bWantToPickUpItem;
	
	return Action;
}

void UActionArmHandToLocation::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	
	const FTransform Xform = MoveArmRef->ArmParts[0]->GetComponentTransform();
	const FVector HandLocationPos = Xform.InverseTransformPosition(MoveArmRef->ArmParts[2]->GetComponentLocation());
	
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));

	const FVector ToLocationPos = Xform.InverseTransformPosition(ToLocation);
	
	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, ToLocationPos));

	AnimTransforms.Add(FTransform(MoveArmRef->TargetPose.Rotation, HandLocationPos));
	
	if (const ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCharacter))
	{
		CompanionPawn->GetCompanionWidgetIndicator()->SetNewIndicatorImage(ECompanionIndicatorImage::ExclamationMark);
	}
}

void UActionArmHandToLocation::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	
	if (const ACompanionPawn* CompanionPawn = Cast<ACompanionPawn>(OwnerCharacter))
	{
		CompanionPawn->GetCompanionWidgetIndicator()->SetNewIndicatorImage(ECompanionIndicatorImage::None);
	}
}

void UActionArmHandToLocation::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);

	const UAnimationCurveSubsystem* AnimSubsystem =  GetWorld()->GetSubsystem<UAnimationCurveSubsystem>();
	
	if (!AnimSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("%s - OnUpdate_Implementation - AnimSubsystem Could Not Be Found"), *this->GetFName().ToString());
		return;
	}
	
	const int BeforeMoveNum = AnimTransforms.Num();
	
	const TTuple<TTuple<TArray<FTransform>, float>,FTransform> NewValues = 
		AnimSubsystem->CustomAnim(DeltaTime, 
			AnimTransforms, 
			ElapsedTime,
			PlayRate);
	
	ElapsedTime = NewValues.Key.Value;
	AnimTransforms = NewValues.Key.Key;
	
	MoveArmRef->TargetPose.Position = NewValues.Value.GetLocation();
	MoveArmRef->TargetPose.Rotation = NewValues.Value.GetRotation();

	if (BeforeMoveNum != AnimTransforms.Num() && bWantToPickUpItem)
	{
		OwnerCharacter->TryToPickUpItem();
	}
}

bool UActionArmHandToLocation::IsDone_Implementation() const
{
	return AnimTransforms.Num() <= 1;
}
