// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/PlayerAttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		OnHealthUpdated.Broadcast(NewValue, OldValue);
		UE_LOG(LogTemp, Display, TEXT("OnHealthUpdated"));
	}

	if (const FGameplayAbilityActorInfo* ActorInfo = GetActorInfo())
	{
		if (Attribute == GetPlayerSpeedAttribute())
		{
			if (UCharacterMovementComponent * CharacterMovementComponent = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent))
			{
				CharacterMovementComponent->MaxWalkSpeed = NewValue;
			}
		}
	}
}
