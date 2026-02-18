// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStack/Actions/ActionMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Player//BaseInteractableCharacter.h"

UActionMoveTo* UActionMoveTo::Create(UObject* Outer, ABaseInteractableCharacter* InCompanionCharacter, const FVector& InTargetLocation, bool InNeedIndicator)
{
	UActionMoveTo* Action = NewObject<UActionMoveTo>(Outer);
	Action->CompanionCharacter = InCompanionCharacter;
	Action->TargetLocation = InTargetLocation;
	Action->bNeedIndicator = InNeedIndicator;
	return Action;
}

void UActionMoveTo::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
	bMoveCompleted = false;
	if (!CompanionCharacter) return;
	
    AAIController* AIController = CompanionCharacter->GetController<AAIController>();
	if (!AIController) return;
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	if (!PathComp) return;

	PathComp->AbortMove(*AIController, FPathFollowingResultFlags::ForcedScript);
	PathComp->OnRequestFinished.AddUObject(this, &UActionMoveTo::OnMoveCompleted);

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(MinDistance);

	FNavPathSharedPtr NavPath;
	MoveRequestID = AIController->MoveTo(MoveRequest, &NavPath);
	if (MoveRequestID != EPathFollowingRequestResult::RequestSuccessful)
	{
		// Extra debug info
		UE_LOG(LogTemp, Error, TEXT("MoveTo FAILED! Pawn=%s Target=%s"), *CompanionCharacter->GetName(),*TargetLocation.ToString());
		
		UE_LOG(LogTemp, Warning, TEXT(" - Controller: %s"), *AIController->GetName());
		UE_LOG(LogTemp, Warning, TEXT(" - Pawn has NavAgentProperties? %s"), CompanionCharacter->GetNavAgentPropertiesRef().IsValid() ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Warning, TEXT(" - PathFollowingComponent Valid: %s"),PathComp ? TEXT("YES") : TEXT("NO"));
		
		bMoveCompleted = true;
	}

	if (!bNeedIndicator) return;
	
	//SpawnIndicator
	IndicatorActor = GetWorld()->SpawnActor<AActor>();
	//IndicatorActor->SetActorLabel("MoveToIndicator");
	USceneComponent* Root = NewObject<USceneComponent>(IndicatorActor);
	Root->RegisterComponent();
	IndicatorActor->SetRootComponent(Root);
	IndicatorActor->SetActorLocation(TargetLocation);
		
	// Inside the spawned actor class (or dynamically added component)
	UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(IndicatorActor);
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetHiddenInGame(false);
	
	static UStaticMesh* ArrowMesh = nullptr;
	if (!ArrowMesh)
	{
		ArrowMesh = LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/InteractiveToolsFramework/Meshes/GizmoArrowHandle.GizmoArrowHandle"));
	}

	Mesh->SetWorldScale3D(FVector(2.0f));
	Mesh->SetRelativeLocation(FVector(0, 0, 1) * Mesh->GetComponentScale() * 100.0f);
	Mesh->SetRelativeRotation(FRotator(-90, 0, 0));
	Mesh->SetStaticMesh(ArrowMesh);
	Mesh->RegisterComponent();
		
	UMaterialInterface*  OutlineSeeThrough = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/ActionStackMat/Mat_OutlineSeeThrough.Mat_OutlineSeeThrough"));
	Mesh->SetMaterial(0,  OutlineSeeThrough);
}

void UActionMoveTo::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	if (!bNeedIndicator) return;
	const float UpAndDown = TargetLocation.Z + FMath::Sin(GetWorld()->GetTimeSeconds() * 5.0f) * 25.0f;
	IndicatorActor->SetActorLocation(FVector(IndicatorActor->GetActorLocation().X, IndicatorActor->GetActorLocation().Y, UpAndDown));
}

bool UActionMoveTo::IsDone_Implementation() const
{
	if (!CompanionCharacter)
	{
		return true;
	}
	return bMoveCompleted;
}

void UActionMoveTo::OnEnd_Implementation()
{
	Super::OnEnd_Implementation();
	if (!bNeedIndicator) return;
	IndicatorActor->Destroy();
}

void UActionMoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	bMoveCompleted = true;
	if (const AAIController* AIController = CompanionCharacter->GetController<AAIController>())
	{
		if (UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent())
		{
			PathComp->OnRequestFinished.RemoveAll(this);
		}
	}
}
