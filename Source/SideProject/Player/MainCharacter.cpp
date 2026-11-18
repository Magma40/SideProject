#include "MainCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SideProject.h"
#include "ActionStack/AnimModelActionStack/AnimModelActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActionStack.h"
#include "ActionStack/AnimModelActionStack/ArmActions/ActionArmHandHolding.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedualAnimation/MovingModel.h"
#include "WorldObjects/CameraManagerMain.h"
#include "WorldObjects/PlayerCamera.h"

AMainCharacter::AMainCharacter() : Super()
{
	//Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	//Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	//Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	//instead of recompiling to adjust them
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
	
	TheMainCameraInScene = APlayerCamera::GetCamera(GetWorld());
	
	DefaultMaxSprintSpeed = MaxSprintSpeed;
}

void AMainCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetCharacterMovement()->IsFalling())
	{
		//Slow rotation while in air
		GetCharacterMovement()->RotationRate = FRotator(0.f, 100.0f, 0.f);
		MaxSprintSpeed = DefaultMaxSprintSpeed  + JumpSpeedForceChange;
	}
	else
	{
		//Normal rotation rate on ground
		GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
		MaxSprintSpeed = DefaultMaxSprintSpeed;
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
	}
	else
	{
		UE_LOG(LogSideProject, Error, 
			TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), 
			*GetNameSafe(this));
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	//Input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMainCharacter::DoMove(float Right, float Forward)
{
	if (TheMainCameraInScene != nullptr)
	{
		//Find out which way is forward
		const FRotator Rotation = 	TheMainCameraInScene->GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		//Get forward and right vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

//Resetting what's needed if the player needs to respawn
void AMainCharacter::RespawnCharacter()
{
	Super::RespawnCharacter();
	ACameraManagerMain* CameraManagerMain = Cast<ACameraManagerMain>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraManagerMain::StaticClass()));
	
	if (!IsValid(CameraManagerMain))
	{
		return;
	}

	//Do camera move transition to player respawn 
	CameraManagerMain->BaseCameraPoint = TheMainCameraInScene->GetCurrentCameraPoint();
	CameraManagerMain->CameraActionStack->PushToCameraPointAction(TheMainCameraInScene->GetCurrentCameraPoint(), 1.0f);

	//If Player is hand holding with AI companion, break it up
	if (const TScriptInterface<IActionInterface> PlayerHandHoldingAction = 
		GetMovingModel()->GetAnimModelActionStack()->GetInterfacedAction(GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_L()->GetAction<UActionArmHandHolding>()))
	{
		GetMovingModel()->GetAnimModelActionStack()->GetArmActionStack_L()->RemoveAction(PlayerHandHoldingAction);
	}
}