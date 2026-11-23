#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActionStack.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ActionStackLogCategory, Log, All);

/// <summary>
/// Action UObject, this will exist behind the scenes of the game.
/// This handles all global logic an Action should do.
/// </summary>
UCLASS(Abstract) 
class SIDEPROJECT_API UAction : public UObject
{
	GENERATED_BODY()

public:
	//Non-templated Create that just creates the object (no typed init)
	static UAction* CreateGeneric(UObject* Outer, const TSubclassOf<UAction> ActionClass)
	{
		if (!IsValid(ActionClass))
		{
			return nullptr;
		}
		
		return NewObject<UAction>(Outer, ActionClass);
	}
	
	//Gets called once after the action has been pushed onto a stack.
	virtual void OnBegin(bool bFirstTime) {} 
	
	//Gets called on update if action is on top of the stack.
	virtual void OnUpdate(float DeltaTime) {}
	
	//Gets called once before the action is about to be removed from the stack.
	virtual void OnEnd() {}
	
	//Check if current action is finished and should be removed from the stack.
	virtual bool IsDone() const { return true; }
	
	//Check if the current action is allowed to be pushed onto a stack
	virtual bool CanBeAdded() const { return true; }

	//FIELDS
protected:
	//Array of actions that this action needs to be on a stack before getting pushed onto stack
	//This is used in CanBeAdded()
	UPROPERTY()
	TArray<TSubclassOf<UAction>> ActionsNeeded;
	
	//Array of actions that block this action from getting pushed onto stack
	//This is used in CanBeAdded()
	UPROPERTY()
	TArray<TSubclassOf<UAction>> ActionBlockers;
};

//Templated factory that constructs a UAction-derived object and calls its typed Init(...) method.
//Requires TAction to derive from UAction and to implement: void Init(Args...).
template <typename TAction, typename... TArgs>
TAction* CreateAction(UObject* Outer, TArgs&&... Args)
{
	static_assert(TIsDerivedFrom<TAction, 
		UAction>::Value, 
		"TAction must derive from UAction");

	//Set outer and GC tracking.
	TAction* New = NewObject<TAction>(Outer);
	if (!IsValid(New))
	{
		return nullptr;
	}

	//Call the strongly-typed Init on the created object.
	New->Init(Forward<TArgs>(Args)...);

	return New;
}

/// <summary>
/// Action Stack UObject, this will exist behind the scenes of the game.
/// This handles all the calls the Action should do.
/// </summary>
UCLASS(Abstract)
class SIDEPROJECT_API UActionStack : public UObject
{
	GENERATED_BODY()
	
	//FUNCTIONS:
public:
	//Non-templated Create that just creates the object (no typed init)
	static UActionStack* CreateGeneric(UObject* Outer, const TSubclassOf<UActionStack> ActionClass)
	{
		if (!IsValid(ActionClass))
		{
			return nullptr;
		}
		
		return NewObject<UActionStack>(Outer, ActionClass);
	}
	
	//Update the top action in the ActionStack
	virtual void UpdateActions(float DeltaTime);
	
	//Push an action onto the top of the ActionStack
	virtual void PushAction(UAction& Action);
	
	//Pop an action from the top of the ActionStack
	virtual void PopAction(UAction& Action);
	
	//Does the ActionStack contain the inserted action class?
	virtual bool Contains(const TSubclassOf<UAction>& InActionClass) const;
	
	//Is the ActionStack empty?
	FORCEINLINE virtual bool IsEmpty() const;
	
	//FIELDS:
private:
	UPROPERTY(Transient) 
	TArray<TObjectPtr<UAction>> ActionStack;
};

//Templated factory that constructs a UActionStack-derived object and calls its typed Init(...) method.
//Requires TActionStack to derive from UActionStack and to implement: void Init(Args...).
template <typename TActionStack, typename... TArgs>
TActionStack* CreateActionStack(UObject* Outer, TArgs&&... Args)
{
	static_assert(TIsDerivedFrom<TActionStack, 
		UActionStack>::Value, 
		"TActionStack must derive from UActionStack");

	//Set outer and GC tracking.
	TActionStack* New = NewObject<TActionStack>(Outer);
	if (!IsValid(New))
	{
		return nullptr;
	}

	//Call the strongly-typed Init on the created object.
	New->Init(Forward<TArgs>(Args)...);

	return New;
}