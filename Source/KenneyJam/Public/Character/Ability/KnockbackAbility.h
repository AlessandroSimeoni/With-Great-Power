// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "KnockbackAbility.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EKnockbackType : uint8
{
	RootMotion UMETA(DisplayName = "Root Motion"),
	Impulse UMETA(DisplayName = "Impulse"),
	Launch UMETA(DisplayName = "Launch Character"),
	BluePrint UMETA(DisplayName = "Use Blueprint"),
};

UENUM(BlueprintType)
enum class EKnockbackDirectionType : uint8
{
	InstigatorForward UMETA(DisplayName = "Instigator Forward"),
	ActorBackward UMETA(DisplayName = "Actor Backward"),
	ActorInstigatorDirection UMETA(DisplayName = "Actor Instigator Direction"),
	
};

UCLASS()
class KENNEYJAM_API UKnockbackAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	

protected:
	virtual void ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
) override;

	UPROPERTY(EditDefaultsOnly,Category="Property")
	EKnockbackType KnockbackType = EKnockbackType::RootMotion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property")
	TSubclassOf<UGameplayEffect> KnockbackEffectClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Property|ForceControl")
	float KnockbackForwardBaseValue;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Property|ForceControl")
	float KnockbackUpBaseValue;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Property|RootControl")
	float RootMotionDuration = 0.5f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Property|RootControl")
	float RootMotionDistance = 100.0f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Property|RootControl")
	TObjectPtr<UCurveFloat> ForceCurve;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Property|KnockbackDirection")
	EKnockbackDirectionType KnockbackDirectionType = EKnockbackDirectionType::InstigatorForward;
	
	
	//KnokcnackFunction
	void ApplyKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude, TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent);
	void ApplyRootMotionKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude);
	void ApplyImpulseKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude);
	void ApplyLaunchKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude);
	void ApplyKnockbackEffect(AActor* TargetCharacterActor,TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent );
	FVector GetKnockbackDirection(AActor* TargetActor,const AActor* InstigatorActor);
};
