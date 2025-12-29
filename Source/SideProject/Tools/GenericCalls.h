#pragma once

namespace GenericCalls
{
	static void GetAllComponentsWithTag(UWorld* World, FName Tag, TArray<USceneComponent*>& OutComponents)
	{
		for (TObjectIterator<USceneComponent> It; It; ++It)
		{
			USceneComponent* Comp = *It;

			if (Comp->GetWorld() == World && Comp->ComponentTags.Contains(Tag))
			{
				OutComponents.Add(Comp);
			}
		}
	}

	inline TArray<USceneComponent*> FindChildrenByPrefix(const USceneComponent* Parent, const FString& Prefix)
	{
		TArray<USceneComponent*> Matches;
		TArray<USceneComponent*> FindChildren;
		Parent->GetChildrenComponents(false, FindChildren);

		for (USceneComponent* Child : FindChildren)
			if (Child->GetName().StartsWith(Prefix))
				Matches.Add(Child);

		return Matches;
	}

	inline USceneComponent* FindChildByExactName(const USceneComponent* Parent, const FString& Target)
	{
		TArray<USceneComponent*> FindChildren;
		Parent->GetChildrenComponents(false, FindChildren);

		for (USceneComponent* Child : FindChildren)
			if (Child->GetName() == Target)return Child;

		return nullptr;
	}

	inline USceneComponent* FindChildByExactName(const AActor* Parent, const FString& Name)
	{
		TArray<USceneComponent*> AllComponents;
		Parent->GetComponents<USceneComponent>(AllComponents);
		for (USceneComponent* Comp : AllComponents)
		{
			if (!Comp) continue;
			if (Comp->GetName() == Name) return Comp;
		}
		return nullptr;
	}

	// TArray<USceneComponent*> FindChildrenByPrefix(const AActor* Parent, const TArray<FString&> Names)
	// {
	// 	TArray<USceneComponent*> FoundByNames;
	// 	//FoundByNames.SetNum(Names.Num());
	// 	TArray<USceneComponent*> AllComponents;
	// 	Parent->GetComponents<USceneComponent>(AllComponents);
	// 	for (USceneComponent* Comp : AllComponents)
	// 	{
	// 		if (!Comp) continue;
	// 		for (int i = 0; i < Names.Num(); ++i)
	// 		{
	// 			if (Names[i] == Comp->GetName()) FoundByNames[0] = Comp;
	// 		}
	// 	}
	// 	return FoundByNames;
	// }
}