#include "WorldInteractions/ObjectInteractions/InteractWeightPlatter.h"

UInteractWeightPlatter::UInteractWeightPlatter()  
{
	PrimaryComponentTick.bCanEverTick = true;
	Type = EActionType::Active;
}


