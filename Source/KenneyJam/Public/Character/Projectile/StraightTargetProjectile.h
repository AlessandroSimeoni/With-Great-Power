// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Projectile/StraightProjectile.h"
#include "StraightTargetProjectile.generated.h"

/**
 * 
 */
UCLASS()
class KENNEYJAM_API AStraightTargetProjectile : public AStraightProjectile
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Settings")
	float MaxAngle = 45.0f;
	
	virtual void Shoot(const UMuzzle* InstigatorMuzzle, const float ProjectileSpeed, const AActor* ProjectileTargetActor) override;
};
