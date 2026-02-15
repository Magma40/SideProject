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
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildrenByPrefix - Parent was not found or is wrong"), "GenericCalls");
			return Matches;
		}
		TArray<USceneComponent*> FindChildren;
		Parent->GetChildrenComponents(false, FindChildren);

		for (USceneComponent* Child : FindChildren)
		{
			if (Child->GetName().StartsWith(Prefix)) Matches.Add(Child);	
		}

		return Matches;
	}

	inline TArray<USceneComponent*> FindChildrenByPrefix(const AActor* Parent, const FString& Prefix)
	{
		TArray<USceneComponent*> Matches;
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildrenByPrefix - Parent was not found or is wrong"), "GenericCalls");
			return Matches;
		}
		TArray<USceneComponent*> FindChildren;
		Parent->GetComponents<USceneComponent>(FindChildren);

		for (USceneComponent* Child : FindChildren)
		{
			if (Child->GetName().StartsWith(Prefix)) Matches.Add(Child);	
		}

		return Matches;
	}

	inline USceneComponent* FindChildByPrefix(const USceneComponent* Parent, const FString& Prefix)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildByPrefix - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		
		TArray<USceneComponent*> FindChildren;
		Parent->GetChildrenComponents(false, FindChildren);

		for (USceneComponent* Child : FindChildren)
		{
			if (Child->GetName().StartsWith(Prefix)) return Child;
		}

		return nullptr;
	}

	inline USceneComponent* FindChildByEndPrefix(const USceneComponent* Parent, const FString& Prefix)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildByEndPrefix - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		
		TArray<USceneComponent*> FindChildren;
		Parent->GetChildrenComponents(false, FindChildren);

		for (USceneComponent* Child : FindChildren)
		{
			if (Child->GetName().EndsWith(Prefix)) return Child;
		}

		return nullptr;
	}

	inline USceneComponent* FindChildByExactName(const USceneComponent* Parent, const FString& Target)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildByExactName - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		
		TArray<USceneComponent*> FindChildren;
		Parent->GetChildrenComponents(false, FindChildren);

		for (USceneComponent* Child : FindChildren)
		{
			if (Child->GetName() == Target)return Child;
		}

		return nullptr;
	}

	inline USceneComponent* FindChildByExactName(const AActor* Parent, const FString& Name)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildByExactName - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		
		TArray<USceneComponent*> AllComponents;
		Parent->GetComponents<USceneComponent>(AllComponents);
		if (AllComponents.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildByExactName - No children was found"), "GenericCalls");
			return nullptr;;
		}
		for (USceneComponent* Comp : AllComponents)
		{
			if (!Comp) continue;
			if (Comp->GetName() == Name) return Comp;
		}
		return nullptr;
	}

	inline AActor* FindChildActorByExactName(const AActor* Parent, const FString& Name)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildActorByExactName - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		TArray<AActor*> AttachedActors;
		Parent->GetAttachedActors(AttachedActors);

		for (AActor* Child : AttachedActors)
		{
			if (!Child) continue;
			if (Child->GetName() == Name)return Child;
		}

		return nullptr;
	}

	template<typename T>
	T* FindChildActorByClass(const AActor* Parent)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildActorByClass - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		TArray<AActor*> AttachedActors;
		Parent->GetAttachedActors(AttachedActors);
		for (AActor* Child : AttachedActors)
		{
			if (!Child) continue;
			if (T* CastChild = Cast<T>(Child)) return CastChild;
		}
		return nullptr;
	}

	template<typename T>
	T* FindChildComponentByClass(AActor* Parent)
	{
		if (!IsValid(Parent))
		{
			UE_LOG(LogTemp, Error, TEXT("%hs:FindChildComponentByClass - Parent was not found or is wrong"), "GenericCalls");
			return nullptr;;
		}
		TArray<USceneComponent*> AllComponents;
		Parent->GetComponents<USceneComponent>(AllComponents);
		for (USceneComponent* Child : AllComponents)
		{
			if (!Child) continue;
			if (T* CastChild = Cast<T>(Child)) return CastChild;
		}
		return nullptr;
	}
}