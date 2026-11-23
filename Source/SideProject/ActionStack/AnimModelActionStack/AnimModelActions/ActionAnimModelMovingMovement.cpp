#include "ActionStack/AnimModelActionStack/AnimModelActions/ActionAnimModelMovingMovement.h"
#include "ProcedualAnimation/MovingModel.h"

void UActionAnimModelMovingMovement::OnUpdate(const float DeltaTime) 
{
	Super::OnUpdate(DeltaTime);
	
	check(IsValid(MovingModel));
	
	MovingModel->UpdateHips();
}