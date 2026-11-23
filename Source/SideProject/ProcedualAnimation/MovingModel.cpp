#include "ProcedualAnimation/MovingModel.h"

#include "Joint.h"
#include "Limb.h"
#include "ProcedualAnimation/MoveLeg.h"
#include "ProcedualAnimation/MoveArm.h"
#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "Tools/GenericCalls.h"

DEFINE_LOG_CATEGORY(ProceduralAnimationLogCategory);

UMovingModel::UMovingModel()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovingModel::BuildProceduralBodyParts()
{
	AActor* Owner = GetOwner();
	
	if (IsValid(Owner))
	{
		UE_LOG(ProceduralAnimationLogCategory, Error, 
			TEXT("%s::BuildProceduralBodyParts() - Owner is not valid - returning"), *GetName());
		return;
	}
	
	for (int32 Instance = 0; Instance < ProceduralBodyParts.Num(); ++Instance)
	{
		FProceduralBodyPart& BodyPart = ProceduralBodyParts[Instance];
		
		BodyPart.Joints.Empty();
		BodyPart.Limbs.Empty();
		
		USceneComponent* LastKnownParent = this;
		
		//Create instances of the joints
		for (const TPair<FVector, bool>& JointsPosition : BodyPart.JointsPositions)
		{
			//Create an instance of the joint
			UJoint* JointInstance =	NewObject<UJoint>(this, UJoint::StaticClass(), TEXT("GenerateJointName(Instance)"));
			
			if (IsValid(JointInstance))
			{
				//Register with the world so it renders and updates
				JointInstance->RegisterComponent();
    
				//Attach to the owner's root component
				JointInstance->AttachToComponent(LastKnownParent, FAttachmentTransformRules::KeepRelativeTransform);
    
				//Ensure the editor structure updates correctly
				Owner->AddInstanceComponent(JointInstance);

				//Set the location
				JointInstance->SetRelativeLocation(JointsPosition.Key);
				
				//Should the joint be visible?
				JointInstance->SetHiddenInGame(JointsPosition.Value);
				
				LastKnownParent = JointInstance;
				
				BodyPart.Joints.Add(JointInstance);
			}
		}
		
		//Create Instances of the limbs
		for (int32 JointIndex = 0; JointIndex < BodyPart.Joints.Num() - 1; ++JointIndex)
		{
			const int32 NextJointIndex = JointIndex + 1;
			
			UJoint* LastJoint = BodyPart.Joints[JointIndex];
			const UJoint* NextJoint = BodyPart.Joints[NextJointIndex];
			
			if (!IsValid(LastJoint) || !IsValid(NextJoint))
			{
				break;
			}
			
			//Create an instance of the limb
			ULimb* LimbInstance = NewObject<ULimb>(this, ULimb::StaticClass(), TEXT("GenerateJointName(Instance)"));
			
			if (IsValid(LimbInstance))
			{
				//Register with the world so it renders and updates
				LimbInstance->RegisterComponent();
    
				//Attach to the owner's root component
				LimbInstance->AttachToComponent(LastJoint, FAttachmentTransformRules::KeepRelativeTransform);
				
				const FVector ParentPos = LastJoint->GetComponentLocation();
				const FVector ChildPos = NextJoint->GetComponentLocation();

				const FVector MidPoint = (ParentPos + ChildPos) * 0.5f;
				const FVector Direction = (ParentPos - ChildPos).GetSafeNormal();
	
				const FRotator LookAtRotation = (Direction.Rotation() + FRotator(90, 0, 0));
	
				SetWorldLocation(MidPoint);
				SetWorldRotation(LookAtRotation);
    
				//Ensure the editor structure updates correctly
				Owner->AddInstanceComponent(LimbInstance);
				
				BodyPart.Limbs.Add(LimbInstance);
			}
		}
	}
}

FName UMovingModel::GenerateJointName(const int32 InstanceIndex) const
{
	if (InstanceIndex % 2) //Left hand instance
	{
		//return FName("%s_L_%i", InTypePart, InstanceIndex); 
	}
	else //Right hand Instance
	{
		//return FName("%s_R_%i", InTypePart, InstanceIndex); 
	}
	return FName();
}

void UMovingModel::InitSceneComponents() //This one is called through UAnimModelActionStack
{
	//Get Hips
	Hips_Ref = GenericCalls::FindChildByExactName(this, TEXT("Hips"));
	
	if (!Hips_Ref)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:InitSceneComponents - Hips not found or wrong type"), *StaticClass()->GetName());
		return;
	}

	//Get Chest
	Chest_Ref = GenericCalls::FindChildByExactName(this, TEXT("Chest"));
	
	if (!Chest_Ref)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:InitSceneComponents - Chest not found or wrong type"),  *StaticClass()->GetName());
		return;
	}

	//Legs and arms chains
	Legs = BuildLimbChains<UMoveLeg>(Hips_Ref,TEXT("UpperLeg_"),TEXT("Knee_"),TEXT("Foot_"));
	Arms = BuildLimbChains<UMoveArm>( Chest_Ref,TEXT("UpperArm_"),TEXT("Elbow_"),TEXT("Hand_"));
}

void UMovingModel::BeginPlay()
{
	Super::BeginPlay();

	//Setup base hip height reference 
	const FTransform ParentXform = GetComponentTransform();
	
	const float HipLocalZ  = ParentXform.InverseTransformPosition(Hips_Ref->GetComponentLocation()).Z;
	const float FootLocalZ = ParentXform.InverseTransformPosition(Legs[0].End->GetComponentLocation()).Z;
	
	BaseHipHeight = HipLocalZ - FootLocalZ;
}

void UMovingModel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
}

void UMovingModel::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//DrawDebugJoints();
	//DrawDebugLimbs();
}

void UMovingModel::UpdateHips()
{
	if (Legs.Num() == 0 ||
		!IsValid(Hips_Ref))
	{
		return;
	}
	
	const float HipSway =  GetHipSway();

	//Simulate a constant vertical hip sway
	const float DesiredHipHeight = BaseHipHeight;
	const float HipHeight = DesiredHipHeight + FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f) * 0.5f;
	const FVector NewHipLocalPos = FVector(0, 0, HipHeight) + HipSway;

	//Apply New Hip Location
	Hips_Ref->SetRelativeLocation(NewHipLocalPos);
}


//Get if the legs are stepping, return hip sway, else don't 
float UMovingModel::GetHipSway()
{
	float HipSway = 0.0f;
	
	//Leg Logic
	for (const FLimbChain& Leg : Legs)
	{
		const UMoveLeg* UpperLeg = Cast<UMoveLeg>(Leg.Upper);
		
		if (!IsValid(UpperLeg))
		{
			continue;
		}
		
		if (UpperLeg->IsStepping())
		{
			const float Lift = UpperLeg->GetLiftAmount();
			const bool IsRight = UpperLeg->GetName().EndsWith(TEXT("_R"));
			
			HipSway = Lift * (IsRight ? 1.f : -1.f);
		}
	}
	
	HipSway = HipSway * 0.1f;
	
	return HipSway;
}

void UMovingModel::SetJumpingAnim() const
{
	if (!IsValid(AnimModelActionStack_Ref))
	{
		return;
	}
	
	AnimModelActionStack_Ref->PushJumpingActionMovement();
}

void UMovingModel::SetArmsWaveAnim() const
{
	if (!IsValid(AnimModelActionStack_Ref))
	{
		return;
	}
	
	AnimModelActionStack_Ref->PushWaveActionMovement();
}
void UMovingModel::SetArmsBackHolsterAnim() const
{
	if (!IsValid(AnimModelActionStack_Ref))
	{
		return;
	}
	
	AnimModelActionStack_Ref->PushHolsterActionMovement();
}

void UMovingModel::SetArmsPointingAnim() const
{
	if (!IsValid(AnimModelActionStack_Ref))
	{
		return;
	}
	
	//AnimModelActionStack_Ref->PushPointingActionMovement();
}

void UMovingModel::RemoveArmsPointingAnim() const
{
	if (!IsValid(AnimModelActionStack_Ref))
	{
		return;
	}
}

void UMovingModel::SetArmsHoldingHandsAnim() const
{
	if (!IsValid(AnimModelActionStack_Ref))
	{
		return;
	}
}

//Debug draw line the procedural animation joints
void UMovingModel::DrawDebugJoints()
{
	FVector Pos_Hips = Hips_Ref->GetComponentLocation();
	FVector Pos_Chest = Chest_Ref->GetComponentLocation();
	
	FVector Pos_UpperLeg_R =	FVector::ZeroVector;
	FVector Pos_Knee_R = FVector::ZeroVector;
	FVector Pos_Foot_R = FVector::ZeroVector;

	FVector Pos_UpperLeg_L = FVector::ZeroVector;
	FVector Pos_Knee_L = FVector::ZeroVector;
	FVector Pos_Foot_L =	FVector::ZeroVector;

	FVector Pos_UpperArm_L =	FVector::ZeroVector;
	FVector Pos_Elbow_L = FVector::ZeroVector;
	FVector Pos_Hand_L = FVector::ZeroVector;

	FVector Pos_UpperArm_R =	FVector::ZeroVector;
	FVector Pos_Elbow_R = FVector::ZeroVector;
	FVector Pos_Hand_R = FVector::ZeroVector;

	for(const FLimbChain& Leg : Legs)
	{
		if (Leg.Upper->GetName().EndsWith(TEXT("_R")))
		{
			Pos_UpperLeg_R = Leg.Upper->GetComponentLocation();
		}
		else
		{
			Pos_UpperLeg_L = Leg.Upper->GetComponentLocation();
		}

		if (Leg.Middle->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Knee_R = Leg.Middle->GetComponentLocation();
		}
		else
		{
			Pos_Knee_L = Leg.Middle->GetComponentLocation();
		}

		if (Leg.End->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Foot_R = Leg.End->GetComponentLocation();
		}
		else
		{
			Pos_Foot_L = Leg.End->GetComponentLocation();
		}
	}

	for(const FLimbChain& Arm : Arms)
	{
		if (Arm.Upper->GetName().EndsWith(TEXT("_R")))
		{
			Pos_UpperArm_R = Arm.Upper->GetComponentLocation();
		}
		else
		{
			Pos_UpperArm_L = Arm.Upper->GetComponentLocation();
		}

		if (Arm.Middle->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Elbow_R = Arm.Middle->GetComponentLocation();
		}
		else
		{
			Pos_Elbow_L = Arm.Middle->GetComponentLocation();
		}

		if (Arm.End->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Hand_R = Arm.End->GetComponentLocation();
		}
		else
		{
			Pos_Hand_L = Arm.End->GetComponentLocation();
		}
	}

	DrawDebugSphere(GetWorld(), Pos_Hips,8.f, 12, FColor::Cyan);
	DrawDebugSphere(GetWorld(), Pos_Chest, 8.f, 12, FColor::Cyan);

	DrawDebugSphere(GetWorld(), Pos_UpperLeg_R,8.f, 12, FColor::Green);
	DrawDebugSphere(GetWorld(), Pos_Knee_R, 8.f, 12, FColor::Emerald);
	DrawDebugSphere(GetWorld(), Pos_Foot_R, 8.f, 12, FColor::Purple);

	DrawDebugSphere(GetWorld(), Pos_UpperLeg_L, 8.f, 12, FColor::Green);
	DrawDebugSphere(GetWorld(), Pos_Knee_L, 8.f, 12, FColor::Emerald);
	DrawDebugSphere(GetWorld(), Pos_Foot_L, 8.f, 12, FColor::Purple);

	DrawDebugSphere(GetWorld(), Pos_UpperArm_R, 8.f, 12, FColor::Yellow);
	DrawDebugSphere(GetWorld(), Pos_Elbow_R, 8.f, 12, FColor::Orange);
	DrawDebugSphere(GetWorld(), Pos_Hand_R, 8.f, 12, FColor::Red);

	DrawDebugSphere(GetWorld(), Pos_UpperArm_L, 8.f, 12, FColor::Yellow);
	DrawDebugSphere(GetWorld(), Pos_Elbow_L, 8.f, 12, FColor::Orange);
	DrawDebugSphere(GetWorld(), Pos_Hand_L, 8.f, 12, FColor::Red);
}

//Debug draw line the procedural animation limbs
void UMovingModel::DrawDebugLimbs()
{
	FVector Pos_Hips = Hips_Ref->GetComponentLocation();
	FVector Pos_Chest = Chest_Ref->GetComponentLocation();
	
	FVector Pos_UpperLeg_R = FVector::ZeroVector;
	FVector Pos_Knee_R = FVector::ZeroVector;
	FVector Pos_Foot_R = FVector::ZeroVector;

	FVector Pos_UpperLeg_L =	FVector::ZeroVector;
	FVector Pos_Knee_L = FVector::ZeroVector;
	FVector Pos_Foot_L =	FVector::ZeroVector;

	FVector Pos_UpperArm_L =	FVector::ZeroVector;
	FVector Pos_Elbow_L = FVector::ZeroVector;
	FVector Pos_Hand_L = FVector::ZeroVector;

	FVector Pos_UpperArm_R =	FVector::ZeroVector;
	FVector Pos_Elbow_R = FVector::ZeroVector;
	FVector Pos_Hand_R = FVector::ZeroVector;

	for(const FLimbChain& Leg : Legs)
	{
		if (Leg.Upper->GetName().EndsWith(TEXT("_R")))
		{
			Pos_UpperLeg_R = Leg.Upper->GetComponentLocation();
		}
		else
		{
			Pos_UpperLeg_L = Leg.Upper->GetComponentLocation();
		}

		if (Leg.Middle->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Knee_R = Leg.Middle->GetComponentLocation();
		}
		else
		{
			Pos_Knee_L = Leg.Middle->GetComponentLocation();
		}

		if (Leg.End->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Foot_R = Leg.End->GetComponentLocation();
		}
		else
		{
			Pos_Foot_L = Leg.End->GetComponentLocation();
		}
	}

	for(const FLimbChain& Arm : Arms)
	{
		if (Arm.Upper->GetName().EndsWith(TEXT("_R")))
		{
			Pos_UpperArm_R = Arm.Upper->GetComponentLocation();
		}
		else
		{
			Pos_UpperArm_L = Arm.Upper->GetComponentLocation();
		}

		if (Arm.Middle->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Elbow_R = Arm.Middle->GetComponentLocation();
		}
		else
		{
			Pos_Elbow_L = Arm.Middle->GetComponentLocation();
		}

		if (Arm.End->GetName().EndsWith(TEXT("_R")))
		{
			Pos_Hand_R = Arm.End->GetComponentLocation();
		}
		else
		{
			Pos_Hand_L = Arm.End->GetComponentLocation();
		}
	}

	DrawDebugLine(GetWorld(), Pos_Hips, Pos_Chest, FColor::Cyan);
	
	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_R,	 FColor::Green);
	DrawDebugLine(GetWorld(), Pos_Hips, Pos_UpperLeg_L, FColor::Green);

	DrawDebugLine(GetWorld(), Pos_Chest, Pos_UpperArm_R,	 FColor::Yellow);
	DrawDebugLine(GetWorld(), Pos_Chest, Pos_UpperArm_L, FColor::Yellow);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_R, Pos_Knee_R, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_R, Pos_Foot_R, FColor::Purple);

	DrawDebugLine(GetWorld(), Pos_UpperLeg_L, Pos_Knee_L, FColor::Emerald);
	DrawDebugLine(GetWorld(), Pos_Knee_L, Pos_Foot_L, FColor::Purple);

	DrawDebugLine(GetWorld(), Pos_UpperArm_R, Pos_Elbow_R, FColor::Orange);
	DrawDebugLine(GetWorld(), Pos_Elbow_R, Pos_Hand_R,  FColor::Red);

	DrawDebugLine(GetWorld(), Pos_UpperArm_L, Pos_Elbow_L, FColor::Orange);
	DrawDebugLine(GetWorld(), Pos_Elbow_L, Pos_Hand_L, FColor::Red);
}

//Builds a limb reference to store arms or legs references
template<typename TUpperClass>
TArray<FLimbChain> UMovingModel::BuildLimbChains( USceneComponent* Root, const FString& UpperPrefix, const FString& MiddlePrefix, const FString& EndPrefix) const
{
	TArray<FLimbChain> OutChains;

	const TArray<USceneComponent*> UpperList = GenericCalls::FindChildrenByPrefix(Root, UpperPrefix);

	for (USceneComponent* UpperComp : UpperList)
	{
		TUpperClass* Upper = Cast<TUpperClass>(UpperComp);
		if (!Upper)
		{
			continue;
		}

		FLimbChain Chain;
		Chain.Upper = Upper;

		TArray<USceneComponent*> Middles = GenericCalls::FindChildrenByPrefix(UpperComp, MiddlePrefix);
		if (Middles.Num() == 0)
		{
			continue;
		}
		
		Chain.Middle = Middles[0];

		TArray<USceneComponent*> Ends = GenericCalls::FindChildrenByPrefix(Chain.Middle, EndPrefix);
		if (Ends.Num() == 0)
		{
			continue;
		}
		
		Chain.End = Ends[0];

		OutChains.Add(Chain);
	}

	return OutChains;
}

USceneComponent* UMovingModel::GetRightHand() const
{
	for(const FLimbChain Arm : Arms)
	{
		if (Arm.End->GetName().EndsWith(TEXT("_R")))
		{
			return Arm.End;
		}
	}
	return nullptr;
}

USceneComponent* UMovingModel::GetLeftHand() const
{
	for(const FLimbChain Arm : Arms)
	{
		if (Arm.End->GetName().EndsWith(TEXT("_L")))
		{
			return Arm.End;
		}
	}
	return nullptr;
}

UMoveArm* UMovingModel::GetRightArm() const
{
	for(const FLimbChain Arm : Arms)
	{
		if (Arm.Upper->GetName().EndsWith(TEXT("_R")))
		{
			return Cast<UMoveArm>(Arm.Upper);
		}
	}
	return nullptr;
}

UMoveArm* UMovingModel::GetLeftArm() const
{
	for(const FLimbChain Arm : Arms)
	{
		if (Arm.Upper->GetName().EndsWith(TEXT("_L")))
		{
			return Cast<UMoveArm>(Arm.Upper);
		}
	}
	return nullptr;
}

TArray<UMoveLeg*> UMovingModel::GetLegs()
{
	TArray<UMoveLeg*> LegsTops;
	
	for(const FLimbChain Leg : Legs)
	{
		LegsTops.Add(Cast<UMoveLeg>(Leg.Upper));
	}
	
	return LegsTops;
}

TArray<UMoveArm*> UMovingModel::GetArms()
{
	TArray<UMoveArm*> ArmsTops;
	
	for(const FLimbChain Arm : Arms)
	{
		ArmsTops.Add(Cast<UMoveArm>(Arm.Upper));
	}
	
	return ArmsTops;
}