#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingModel.generated.h"

class ULimb;
class UJoint;
class UActionStack;

DECLARE_LOG_CATEGORY_EXTERN(ProceduralAnimationLogCategory, Log, All);

USTRUCT()
struct FLimbChain
{
	GENERATED_BODY()

	UPROPERTY()  USceneComponent* Upper = nullptr; // UpperLeg or UpperArm
	UPROPERTY()  USceneComponent* Middle = nullptr; // Knee or Elbow
	UPROPERTY()  USceneComponent* End = nullptr; // Foot or Hand
};

USTRUCT(BlueprintType)
struct FProceduralBodyPart
{
	GENERATED_BODY()
	
public:
	//Type of action stack class this body part should replicate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UActionStack> ActionStackType = nullptr;
	
	//Positions of the joints for the body part.
	//True = mesh is visible, False = mesh is invisible. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FVector, bool> JointsPositions = {};
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UJoint>> Joints = TArray<TObjectPtr<UJoint>>();
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<ULimb>> Limbs = TArray<TObjectPtr<ULimb>>();
};

class UAnimModelActionStack;
class UMoveLeg;
class UMoveArm;

/**
 The class for the hips in procedural animation
 */
UCLASS()
class SIDEPROJECT_API UMovingModel : public USceneComponent
{
	GENERATED_BODY()
	
	UMovingModel();

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
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void BuildProceduralBodyParts();
	
	FName GenerateJointName(const int32 InstanceIndex) const;
	
	void DrawDebugJoints();
	
	void DrawDebugLimbs();

	//FIELDS:
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hips") 
	float HipSwayAmount = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hips") 
	float MoveSpeed = 1.0f; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hips") 
	float RotationSpeed = 100.0f;
	
	//Procedural body parts.
	//Key defines where first joint should start from.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyParts") 
	TArray<FProceduralBodyPart> ProceduralBodyParts;

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
