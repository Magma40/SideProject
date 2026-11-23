#include "ActionStack/ActionStack.h"

DEFINE_LOG_CATEGORY(ActionStackLogCategory);

void UActionStack::UpdateActions(const float DeltaTime)
{
	//If stack is empty, no need to update any actions.
	if (IsEmpty())
	{
		return;
	}
	
	//Only update the top action.
	//If action is finished, remove it from the stack.
	if (IsValid(ActionStack.Last()) && 
		!ActionStack.Last()->IsDone())
	{
		ActionStack.Last()->OnUpdate(DeltaTime);
	}
	else
	{
		PopAction(*ActionStack.Last());
	}
}

void UActionStack::PushAction(UAction& Action)
{
	//No need to add the action if it's already on the stack
	if (Contains(Action.GetClass()))
	{
		const FString FailureReason = IsValid(Action.GetClass()) 
		? FString::Printf(TEXT("Action (%s) is already on the stack"), *Action.GetClass()->GetName()) 
		: TEXT("Action was not valid");
		
		UE_LOG(ActionStackLogCategory, Warning, TEXT("%s::PushAction() - Failed To Push Action: \n%s"), *GetName(), *FailureReason);
		
		return;
	}
	
	//Call OnBegin on the action before adding it 
	Action.OnBegin(true);
	
	//Insert action to the top of the stack
	ActionStack.Push(&Action);
}

void UActionStack::PopAction(UAction& Action)
{
	//No need to remove an action if it's not on the stack
	if (!Contains(Action.GetClass()))
	{
		const FString FailureReason = IsValid(Action.GetClass()) 
		? FString::Printf(TEXT("Action (%s) was not on the stack"), *Action.GetClass()->GetName()) 
		: TEXT("Action was not valid");
		
		UE_LOG(ActionStackLogCategory, Warning, TEXT("%s::PushAction() - Failed To Push Action: \n%s"), *GetName(), *FailureReason);
		
		return;
	}
	
	//Call OnEnd on the action before removing it
	Action.OnEnd();
	
	//Remove action from the stack
	ActionStack.Pop();
}

bool UActionStack::IsEmpty() const
{
	return ActionStack.Num() == 0;
}

bool UActionStack::Contains(const TSubclassOf<UAction>& InActionClass) const
{
	if (!IsValid(InActionClass))
	{
		return false;
	}

	//Do an iterator,
	//We use ContainsByPredicate since the stack can only contain a single action of each class.
	return ActionStack.ContainsByPredicate(
		[InActionClass](const TObjectPtr<UAction>& ExistingAction)
	{
		return IsValid(ExistingAction) && ExistingAction->IsA(InActionClass);
	});
}