// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (TryGetPawnOwner())
	{
		PlayerVelocity = TryGetPawnOwner()->GetMovementComponent()->Velocity.SquaredLength();
	}
}
