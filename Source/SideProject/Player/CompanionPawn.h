#pragma once

#include "CoreMinimal.h"
#include "Player/BaseInteractableCharacter.h"
#include "CompanionPawn.generated.h"

class UCompanionWidgetIndicator;
class UInteractionBehaviour;

/**
 Class for the companion AI
*/

UCLASS()
class SIDEPROJECT_API ACompanionPawn : public ABaseInteractableCharacter
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	UFUNCTION(BlueprintCallable) 
	virtual void RespawnCharacter() override;
	
	UFUNCTION(BlueprintCallable) 
	void RunLastKnowInteract() const;
	
	virtual void BeginPlay() override;
	
	//~ Begin Getters & Setters
	FORCEINLINE void SetLastKnownInteractionBehaviour(UInteractionBehaviour* InLastKnownInteractionBehaviour)
	{
		LastKnownInteractionBehaviour = InLastKnownInteractionBehaviour;
	}
	
	FORCEINLINE UCompanionWidgetIndicator* GetCompanionWidgetIndicator() const
	{
		return CompanionWidgetIndicator;
	}
	
	FORCEINLINE void SetCompanionWidgetIndicator(UCompanionWidgetIndicator* InNewWidgetIndicator)
	{
		CompanionWidgetIndicator = InNewWidgetIndicator;
	}
	//~ End Getters & Setters

	//FIELDS:
protected:
	UPROPERTY() 
	TObjectPtr<UCompanionWidgetIndicator> CompanionWidgetIndicator = nullptr;
	
	UPROPERTY() 
	TObjectPtr<UInteractionBehaviour> LastKnownInteractionBehaviour = nullptr;
};