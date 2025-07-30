// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

UCLASS()
class KENNEYJAM_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion|Idle to Run")
	float PlayerVelocity = 0.0f;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
