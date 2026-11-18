#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingModel.generated.h"

USTRUCT()
struct FLimbChain
{
	GENERATED_BODY()

	UPROPERTY()  USceneComponent* Upper = nullptr; // UpperLeg or UpperArm
	UPROPERTY()  USceneComponent* Middle = nullptr; // Knee or Elbow
	UPROPERTY()  USceneComponent* End = nullptr; // Foot or Hand
};

class UAnimModelActionStack;
class UMoveLeg;
class UMoveArm;

/**
 The class for the hips in procedural animation
 */
UCLASS()
class SIDEPROJECT_API AMovingModel : public AActor
{
	GENERATED_BODY()
	
	AMovingModel();

	//FUNCTIONS
public:
	void InitSceneComponents();
	
	void UpdateHips();
	
	float GetHipSway();

	UFUNCTION(BlueprintCallable) 
	void SetJumpingAnim() const;
	
	UFUNCTION(BlueprintCallable) 
	void SetArmsWaveAnim() const;
	
	UFUNCTION(BlueprintCallable)
	void SetArmsBackHolsterAnim() const;
	
	UFUNCTION(BlueprintCallable) 
	void SetArmsPointingAnim() const;
	
	UFUNCTION(BlueprintCallable) 
	void SetArmsHoldingHandsAnim() const;
	
	UFUNCTION(BlueprintCallable) 
	void RemoveArmsPointingAnim() const;
	
	template<typename TUpperClass>
	TArray<FLimbChain> BuildLimbChains(USceneComponent* Root, 
		const FString& UpperPrefix, 
		const FString& MiddlePrefix, 
		const FString& EndPrefix) const;
	
	//~ Begin Getters & Setters
	UFUNCTION(BlueprintCallable) 
	FORCEINLINE USceneComponent* GetRightHand() const;
	
	UFUNCTION(BlueprintCallable) 
	FORCEINLINE USceneComponent* GetLeftHand() const;
	
	FORCEINLINE UMoveArm* GetRightArm() const;
	
	FORCEINLINE UMoveArm* GetLeftArm() const;
	
	FORCEINLINE TArray<UMoveLeg*> GetLegs();
	
	FORCEINLINE TArray<UMoveArm*> GetArms();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAnimModelActionStack* GetAnimModelActionStack() const
	{
		return AnimModelActionStack_Ref;
	}
	
	FORCEINLINE void SetAnimModelActionStack(UAnimModelActionStack* InAnimModelActionStack)
	{
		AnimModelActionStack_Ref = InAnimModelActionStack;
	} 
	//~ End Getters & Setters
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	void DrawDebugJoints();
	
	void DrawDebugLimbs();

	//FIELDS:
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") 
	float HipSwayAmount = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") 
	float MoveSpeed = 1.0f; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hips") 
	float RotationSpeed = 100.0f;

private:
	float BaseHipHeight = 0.0f;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> Hips_Ref = nullptr;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> Chest_Ref = nullptr;
	
	UPROPERTY() 
	TArray<FLimbChain> Legs;
	
	UPROPERTY() 
	TArray<FLimbChain> Arms;

	UPROPERTY()
	TObjectPtr<UAnimModelActionStack> AnimModelActionStack_Ref = nullptr;
};
