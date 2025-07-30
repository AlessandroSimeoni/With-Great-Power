// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/KnockbackAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

void UKnockbackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthority(&ActivationInfo)) return;
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	if (KnockbackType == EKnockbackType::BluePrint)
	{
		if (TriggerEventData)
		{
			K2_ActivateAbilityFromEvent(*TriggerEventData);
			return;
		}

		K2_ActivateAbility();
		return;
	}

	ApplyKnockback(ActorInfo->AvatarActor.Get(),TriggerEventData->Instigator,TriggerEventData->EventMagnitude, ActorInfo->AbilitySystemComponent);

	UE_LOG(LogTemp,Warning,TEXT("KnockbackAbility:: %s"),*TriggerEventData->Instigator->GetName());
	K2_ActivateAbilityFromEvent(*TriggerEventData);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void UKnockbackAbility::ApplyKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude, TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent)
{
	if (!TargetCharacterActor || !InstigatorActor) return;

	switch (KnockbackType)
	{
	case EKnockbackType::RootMotion:
		ApplyRootMotionKnockback(TargetCharacterActor, InstigatorActor, Magnitude);
		break;
        
	case EKnockbackType::Impulse:
		ApplyImpulseKnockback(TargetCharacterActor, InstigatorActor, Magnitude);
		break;
        
	case EKnockbackType::Launch:
		ApplyLaunchKnockback(TargetCharacterActor, InstigatorActor, Magnitude);
		break;
        
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown KnockbackType"));
		break;
	}
	ApplyKnockbackEffect(TargetCharacterActor,AbilitySystemComponent);
}

void UKnockbackAbility::ApplyRootMotionKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude)
{
	if (!TargetCharacterActor || !InstigatorActor) return;
	
	ACharacter* Character = Cast<ACharacter>(TargetCharacterActor);
	if (!Character) return;

	UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
	if (!MovementComp) return;

	FVector Direction = GetKnockbackDirection(TargetCharacterActor,InstigatorActor);;
	
	FVector KnockbackVelocity = Direction * (RootMotionDistance / RootMotionDuration);
	
	TSharedPtr<FRootMotionSource_ConstantForce> RootMotion = MakeShared<FRootMotionSource_ConstantForce>();
	RootMotion->InstanceName = "Knockback";
	RootMotion->Priority = 5;
	RootMotion->Force = KnockbackVelocity * Magnitude;
	RootMotion->Duration = RootMotionDuration;
	RootMotion->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	RootMotion->StrengthOverTime;
	if (ForceCurve)
		RootMotion->StrengthOverTime = ForceCurve;
	
	uint16 RootMotionID = MovementComp->ApplyRootMotionSource(RootMotion);

	// Debug Log
	UE_LOG(LogTemp, Warning, TEXT("Knockback applied. Direction: %s, Magnitude: %f"), *KnockbackVelocity.ToString(), Magnitude);
	
}

void UKnockbackAbility::ApplyImpulseKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor,
	float Magnitude)
{
	if (!TargetCharacterActor || !InstigatorActor) return;

	ACharacter* Character = Cast<ACharacter>(TargetCharacterActor);
	if (!Character) return;

	UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
	if (!MovementComp) return;
	
	FVector HorizontalDirection = GetKnockbackDirection(TargetCharacterActor,InstigatorActor);; 
	
	FVector UpDirection = TargetCharacterActor->GetActorUpVector() * KnockbackUpBaseValue;
	
	FVector KnockbackDirection = (HorizontalDirection + UpDirection) * Magnitude;
	
	MovementComp->AddImpulse(KnockbackDirection, true);

	
	UE_LOG(LogTemp, Warning, TEXT("SetVelocity Knockback applied. Direction: %s, Magnitude: %f"), *KnockbackDirection.ToString(), Magnitude);
}

void UKnockbackAbility::ApplyLaunchKnockback(AActor* TargetCharacterActor, const AActor* InstigatorActor, float Magnitude)
{
	if (!TargetCharacterActor || !InstigatorActor) return;

	ACharacter* Character = Cast<ACharacter>(TargetCharacterActor);
	if (!Character) return;

	FVector HorizontalDirection = GetKnockbackDirection(TargetCharacterActor,InstigatorActor);
	FVector UpDirection = TargetCharacterActor->GetActorUpVector() * KnockbackUpBaseValue;
	FVector KnockbackDirection = (HorizontalDirection + UpDirection) * Magnitude;


	Character->LaunchCharacter(KnockbackDirection, true, true);
	
	UE_LOG(LogTemp, Warning, TEXT("LaunchCharacter Knockback applied. Direction: %s, Magnitude: %f"), *KnockbackDirection.ToString(), Magnitude);
}

void UKnockbackAbility::ApplyKnockbackEffect(AActor* TargetCharacterActor, TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent)
{
	if (!TargetCharacterActor || !KnockbackEffectClass) return;




	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	
	FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(KnockbackEffectClass, 1.0f, EffectContext);
	if (EffectSpec.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
		UE_LOG(LogTemp, Warning, TEXT("KnockbackEffect applied to %s"), *TargetCharacterActor->GetName());
	}
}

FVector UKnockbackAbility::GetKnockbackDirection( AActor* TargetActor,const AActor* InstigatorActor)
{
	FVector HorizontalDirection = FVector::ZeroVector;
	switch (KnockbackDirectionType)
	{
	case EKnockbackDirectionType::ActorBackward:
		HorizontalDirection = -InstigatorActor->GetActorForwardVector() * KnockbackForwardBaseValue;
		break;
	case EKnockbackDirectionType::InstigatorForward:
		HorizontalDirection = InstigatorActor->GetActorForwardVector() * KnockbackForwardBaseValue;
		break;
	case EKnockbackDirectionType::ActorInstigatorDirection:
		HorizontalDirection = TargetActor->GetActorLocation() - InstigatorActor->GetActorLocation();
		HorizontalDirection.Z = 0.0f;
		HorizontalDirection.Normalize();
		HorizontalDirection *= KnockbackForwardBaseValue;
		break;
	}
	return HorizontalDirection;
}
