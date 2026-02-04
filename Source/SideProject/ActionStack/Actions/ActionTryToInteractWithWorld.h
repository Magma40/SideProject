#pragma once

#include "CoreMinimal.h"
#include "ActionStack/ActionStack.h"
#include "ActionTryToInteractWithWorld.generated.h"

class ABaseInteractableCharacter;

UCLASS()
class SIDEPROJECT_API UActionTryToInteractWithWorld : public UAction
{
	GENERATED_BODY()
	
public:
	static UActionTryToInteractWithWorld* Create(UObject* Outer, ABaseInteractableCharacter* InCompanionCharacter);
	void OnUpdate_Implementation(float DeltaTime) override;
	bool IsDone_Implementation() const override;
	void OnBegin_Implementation(bool bFirstTime) override;

private:
	ABaseInteractableCharacter* CompanionCharacter = nullptr;
	bool TriedToInteractWithWorld = false;
};
