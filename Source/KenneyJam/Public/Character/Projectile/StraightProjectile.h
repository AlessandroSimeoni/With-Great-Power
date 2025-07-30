// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Projectile/BaseProjectile.h"
#include "StraightProjectile.generated.h"

/**
 * 
 */
UCLASS()
class KENNEYJAM_API AStraightProjectile : public ABaseProjectile
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;
	
public:
	virtual void Shoot(const UMuzzle* InstigatorMuzzle, const float ProjectileSpeed, const AActor* ProjectileTargetActor) override;
};
