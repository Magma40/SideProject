// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedualAnimation/BaseMovingBody.h"

UBaseMovingBody::UBaseMovingBody() { PrimaryComponentTick.bCanEverTick = true; }

void UBaseMovingBody::BeginPlay()
{
	Super::BeginPlay();
	BuildBody();
}

void UBaseMovingBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	float Time = GetWorld()->GetTimeSeconds();
	for (int i = 0; i < BodyParts.Num(); i++)
	{
		if (BodyParts[i].Mesh)
		{
			FVector Offset = BodyParts[i].Offset;
			Offset.Z += FMath::Sin(Time * 2.f + i) * 10.f;
			BodyParts[i].Mesh->SetRelativeLocation(Offset);
		}
	}
	DrawDebugSphere(GetWorld(), HipWorldPos, 8.f, 12, FColor::Cyan);
	DrawDebugLine(GetWorld(), HipPos, NewKneePos, FColor::Red);

}

void UBaseMovingBody::BuildBody()
{
	for (int i = 0; i < 5; i++)
	{
		UStaticMeshComponent* Joint = NewObject<UStaticMeshComponent>(GetOwner());
		Joint->SetupAttachment(this);
		Joint->RegisterComponent();

		// Example: assign a sphere mesh from Engine
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
		if (SphereMesh.Succeeded()) Joint->SetStaticMesh(SphereMesh.Object);
		Joint->SetRelativeLocation(FVector(i * 50.f, 0.f, 0.f));
		BodyParts.Add({Joint, FVector(i * 50.f, 0.f, 0.f), 50.f});
	}
}

