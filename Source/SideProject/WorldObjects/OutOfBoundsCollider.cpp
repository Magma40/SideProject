#include "OutOfBoundsCollider.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "UObject/Object.h"
#include "Player/MainCharacter.h"

AOutOfBoundsCollider::AOutOfBoundsCollider()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	SetRootComponent(RootComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->InitBoxExtent(FVector(50.f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
	BoxComponent->SetCanEverAffectNavigation(false);
}

void AOutOfBoundsCollider::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AOutOfBoundsCollider::OverlapBegin);
}

void AOutOfBoundsCollider::OverlapBegin(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	ABaseInteractableCharacter* TempChar = Cast<ABaseInteractableCharacter>(OtherActor);
	
	if (!TempChar)
	{
		return;
	}
	
	if (Cast<UCapsuleComponent>(OtherComp) != TempChar->GetCapsuleComponent())
	{
		return;
	}
	
	TempChar->RespawnCharacter();
}