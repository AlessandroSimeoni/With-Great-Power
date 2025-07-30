// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Projectile/StraightProjectile.h"

void AStraightProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CanShoot)
		return;

	
	if (SphereDetect())
		HandleCollision();
	
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * Speed * DeltaSeconds);
}

void AStraightProjectile::Shoot(const UMuzzle* InstigatorMuzzle, const float ProjectileSpeed, const AActor* ProjectileTargetActor)
{
	Super::Shoot(InstigatorMuzzle, ProjectileSpeed, ProjectileTargetActor);
	
	Speed = ProjectileSpeed;
	CanShoot = true;
}
