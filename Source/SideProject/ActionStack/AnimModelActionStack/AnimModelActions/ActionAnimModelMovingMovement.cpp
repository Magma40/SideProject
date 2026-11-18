#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"
#include "ProcedualAnimation/MovingModel.h"

UActionAnimModelMovingMovement* UActionAnimModelMovingMovement::Create(UObject* Outer, 
	AMovingModel* InMovingModel)
{
	UActionAnimModelMovingMovement* Action = NewObject<UActionAnimModelMovingMovement>(Outer);
	
	Action->MovingModelRef = InMovingModel;
	
	return Action;
}

void UActionAnimModelMovingMovement::OnBegin_Implementation(const bool bFirstTime)
{
	Super::OnBegin_Implementation(bFirstTime);
}

void UActionAnimModelMovingMovement::OnUpdate_Implementation(const float DeltaTime) 
{
	Super::OnUpdate_Implementation(DeltaTime);
	
	MovingModelRef->UpdateHips();
}

bool UActionAnimModelMovingMovement::IsDone_Implementation() const
{
	return false;
}
