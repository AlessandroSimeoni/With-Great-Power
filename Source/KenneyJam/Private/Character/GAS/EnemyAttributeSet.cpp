// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/EnemyAttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"


void UEnemyAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (const FGameplayAbilityActorInfo* ActorInfo = GetActorInfo())
	{
		if (Attribute == GetSpeedAttribute())
		{
			if (UCharacterMovementComponent * CharacterMovementComponent = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent))
			{
				CharacterMovementComponent->MaxWalkSpeed = NewValue;
				CharacterMovementComponent->MaxFlySpeed = NewValue;
			
			}
		}
	}
}
