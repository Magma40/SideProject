// Copyright Epic Games, Inc. All Rights Reserved.
#include "MainCharacter.h"

#include <ios>

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SideProject.h"
#include "VectorTypes.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MovingModel.h"

AMainCharacter::AMainCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	AActor* FoundTheMainCameraInScene = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass());
	TheMainCameraInScene = Cast<ACameraActor>(FoundTheMainCameraInScene);
	DefaultMaxSprintSpeed = MaxSprintSpeed;

	TArray<AActor*> ChildrenActors;
	GetAllChildActors(ChildrenActors, true);
	for (AActor* Comp : ChildrenActors)
	{
		if (!Comp) continue;
		MovingModel = Cast<AMovingModel>(Comp);
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TheMainCameraInScene != nullptr && !EnabledInput && GetCharacterMovement()->IsFalling())
	{
			// Slow rotation while in air
			GetCharacterMovement()->RotationRate = FRotator(0.f, 100.0f, 0.f);
			MaxSprintSpeed = DefaultMaxSprintSpeed  + JumpSpeedForceChange;
	}
	else
	{
		// Normal rotation rate on ground
		GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
		MaxSprintSpeed = DefaultMaxSprintSpeed;
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
	}
	else
	{
		UE_LOG(LogSideProject, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
	if (EnabledInput)
	{
		LastOldRightValue = MovementVector.X;
		LastOldForwardValue = MovementVector.Y;	
	}
}

void AMainCharacter::DoMove(float Right, float Forward)
{
	if (TheMainCameraInScene != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = TheMainCameraInScene->GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		// get forward and right vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMainCharacter::ContinuousFalling(float Right, float Forward)
{
		EnabledInput = false;
		const FRotator Rotation = TheMainCameraInScene->GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
}

bool AMainCharacter::AbleToPickUpItem()
{
		if (IsValid(ClosestActor) || IsValid(ClosestComponent)) return false;
		return true;
}

bool AMainCharacter::TryToPickUpItem() // Is this even needed?
{
	LastKnownClosestDistance = FLT_MAX;
	ClosestActor = nullptr;
	ClosestComponent = nullptr;
	
	TArray<AActor*> ActorsAround;
	UGameplayStatics::GetAllActorsWithTag (GetWorld(), "HandHeldItem", ActorsAround);
	if (ActorsAround.Num() > 0)
	{
		for(AActor* FoundActor : ActorsAround)
		{
			AActor*  NewClosestActor = GetClosestActorItem(FoundActor);
			if (NewClosestActor != nullptr) ClosestActor =NewClosestActor;
		}
	}

	TArray<USceneComponent*> ComponentsAround;
	GetAllComponentsWithTag(GetWorld(), "HandHeldItem", ComponentsAround);
	if (ComponentsAround.Num() > 0)
	{
		for(USceneComponent* FoundComponent : ComponentsAround)
		{
			USceneComponent*  NewClosestComponent = GetClosestComponentItem(FoundComponent);
			if (NewClosestComponent != nullptr) ClosestComponent = NewClosestComponent;
		}
	}
	
	if (IsValid(ClosestActor) && IsValid(ClosestComponent)) // This is getting too bloated
	{
		float ActorDistance =		 UE::Geometry::Distance(this->GetActorLocation(), ClosestActor->GetActorLocation());
		float ComponentDistance = UE::Geometry::Distance(this->GetActorLocation(), ClosestComponent->GetComponentLocation());
		if (ActorDistance < ComponentDistance)
		{
			ClosestComponent = nullptr;
			if (ActorDistance <= MinDistanceToPickUpItem) return true;
		}
		else if (ComponentDistance < ActorDistance)
		{
			ClosestActor = nullptr;
			if (ComponentDistance <= MinDistanceToPickUpItem) return true;
		}	
	}
	else if (IsValid(ClosestActor) && !IsValid(ClosestComponent))
	{
		float ActorDistance = UE::Geometry::Distance(this->GetActorLocation(), ClosestActor->GetActorLocation());
		if (ActorDistance <= MinDistanceToPickUpItem) return true;
	}
	else if (IsValid(ClosestComponent) && !IsValid(ClosestActor))
	{
		float ComponentDistance = UE::Geometry::Distance(this->GetActorLocation(), ClosestComponent->GetComponentLocation());
		if (ComponentDistance <= MinDistanceToPickUpItem) return true;
	}
	
	return false;
}

bool AMainCharacter::TryToInsertItemIntoSocket()
{
	return true;
	// if (IsValid(ClosestSocket))
	// {
	// 	if (IsValid(ClosestSocket->CurrentActorInSocket))
	// 	{
	// 		AQuestItemScript* SocketsCastItem = Cast< AQuestItemScript>(ClosestSocket->CurrentActorInSocket);
	// 		SocketsCastItem->SetActorTransform(SocketsCastItem->InitTransform);
	// 	}
	// 	return true;
	// 	// if (ClosestSocket->CurrentActorInSocket == nullptr && IsInRangeToSocket && !ClosestSocket->SocketIsFrozen)
	// 	// {
	// 	// 	return true;
	// 	// }
	// 	// else return false; // this will drop the item
	// }
	// else
	// {
	// 	return false;
	// }
}

void AMainCharacter::DropItemInHand()
{
	if (!GetCharacterMovement()->IsFalling() && (IsValid(ClosestActor) || IsValid(ClosestComponent)))
	{
		FVector Start = GetActorLocation() + FVector(0, 0, 50); 
		FVector End = Start + (GetActorForwardVector() * 100.0f);
		
		FHitResult HitResult(ForceInit);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		CollisionParams.AddIgnoredActor(ClosestActor);
		CollisionParams.AddIgnoredComponent(Cast<UPrimitiveComponent>(ClosestComponent));
		CollisionParams.bTraceComplex = true;
		
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility,  CollisionParams );
		if (!bHit) // Only drop if nothing was hit
		{
			if (IsValid(ClosestActor) && !IsValid(ClosestComponent))
			{
				USceneComponent* RootComp = ClosestActor->GetRootComponent();
				if (!IsValid(RootComp)) return;
				
				ClosestActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				ClosestActor->SetActorLocation(End);
				EnablePhysicsRecursively(RootComp);
				ClosestActor = nullptr;
			}
			else if (IsValid(ClosestComponent) &&  !IsValid(ClosestActor))
			{
				AActor* ComponentOwner = ClosestComponent->GetOwner();
				USceneComponent* RootComp = ComponentOwner->GetRootComponent();
				if (!IsValid(ComponentOwner) || !IsValid(RootComp)) return;
				
				ComponentOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				ComponentOwner->SetActorLocation(End);
				EnablePhysicsRecursively(RootComp);
				ClosestComponent = nullptr;
			}
		}
	}
}
	
void AMainCharacter::PickUpItemToHand()
{
	if (IsValid(ClosestActor) && !IsValid(ClosestComponent))
	{
		USceneComponent* RootComp = ClosestActor->GetRootComponent();
		if (!IsValid(RootComp)) return;
		
	    DisablePhysicsRecursively(RootComp);
		ClosestActor->AttachToComponent(MovingModel->GetRightHand(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else if (IsValid(ClosestComponent) &&  !IsValid(ClosestActor))
	{
		AActor*ComponentOwner = ClosestComponent->GetOwner();
		USceneComponent* RootComp = ComponentOwner->GetRootComponent();
		if (!IsValid(ComponentOwner) || !IsValid(RootComp)) return;
		
		DisablePhysicsRecursively(RootComp);
		ComponentOwner->AttachToComponent(MovingModel->GetRightHand(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}
	
//Should Place down item into Socket
void AMainCharacter::InsertItemIntoSocket()
{
	//ClosestSocket->AttachToActor(PickedUpItem);
	//ClosestSocket->UpdateQuest(); //This is not in c++ yet
	//PickedUpItem = nullptr;
}

AActor* AMainCharacter::GetClosestActorItem(AActor* InActor)
{
	if (IsValid(InActor) && !InActor->IsHidden())
	{
		float DistanceToItem = GetDistanceTo(InActor);
		if (DistanceToItem < LastKnownClosestDistance)
		{
			LastKnownClosestDistance = DistanceToItem;
			return InActor;
		}
	}
	return nullptr;
}

USceneComponent* AMainCharacter::GetClosestComponentItem(USceneComponent* InComponent)
{
	if (IsValid(InComponent))
	{
		float DistanceToItem =UE::Geometry::Distance(this->GetActorLocation(), InComponent->GetComponentLocation());
		if (DistanceToItem < LastKnownClosestDistance)
		{
			LastKnownClosestDistance = DistanceToItem;
			return InComponent;
		}
	}
	return nullptr;
}

UPrimitiveComponent* AMainCharacter::GetPrimitiveFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;

	TArray<USceneComponent*> Components;
	Actor->GetComponents<USceneComponent>(Components);

	for (USceneComponent* Comp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
		{
			return Prim; // first found
		}

		// check children recursively
		TArray<USceneComponent*> Children;
		Comp->GetChildrenComponents(true, Children);
		for (USceneComponent* ChildComp : Children)
		{
			if (UPrimitiveComponent* PrimChild = Cast<UPrimitiveComponent>(ChildComp))
			{
				return PrimChild;
			}
		}
	}

	return nullptr;
}

void AMainCharacter::DisablePhysicsRecursively(USceneComponent* Comp)
{
	if (!Comp) return;

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
	{
		Prim->SetMobility(EComponentMobility::Movable);
		Prim->SetSimulatePhysics(false);
		Prim->SetEnableGravity(false);
		Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TArray<USceneComponent*> Children;
	Comp->GetChildrenComponents(true, Children);
	for (USceneComponent* Child : Children)
	{
		DisablePhysicsRecursively(Child);
	}
}

void AMainCharacter::EnablePhysicsRecursively(USceneComponent* Comp)
{
	if (!Comp) return;

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
	{
		Prim->SetMobility(EComponentMobility::Movable);
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	TArray<USceneComponent*> Children;
	Comp->GetChildrenComponents(true, Children);
	for (USceneComponent* Child : Children)
	{
		EnablePhysicsRecursively(Child);
	}
}

