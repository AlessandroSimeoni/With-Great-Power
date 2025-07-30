// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Projectile/Muzzle.h"

#include "Character/Projectile/BaseProjectile.h"

ABaseProjectile*  UMuzzle::Shoot(const TSubclassOf<ABaseProjectile> ProjectileClass, const float ProjectileSpeed, const AActor* ProjectileTargetActor,const float Damage, const float Destruction, const float KnockbackIntensity, const float RandomAngle)
{
	if(!ReadyToShoot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Muzzle %s not ready to shoot"), *GetName());	
		return nullptr;
	}
	
	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, GetComponentLocation(), GetComponentRotation());
	Projectile->SetDamage(Damage);
	Projectile->SetDestruction(Destruction);
	Projectile->Shoot(this, ProjectileSpeed, ProjectileTargetActor);
	Projectile->SetKnockbackIntensity(KnockbackIntensity);
	if (MuzzleCooldownTime > 0.0f)
	{
		ReadyToShoot = false;
		FTimerManager& TM = GetWorld()->GetTimerManager();
		TM.SetTimer(ShootCooldownTimerHandle, this, &ThisClass::HandleCooldownOver, MuzzleCooldownTime, false);
	}
	return Projectile;
}

bool UMuzzle::IsReadyToShoot()
{
	return ReadyToShoot;
}

int32 UMuzzle::GetMuzzleID()
{
	 return MuzzleID;
}


void UMuzzle::HandleCooldownOver()
{
	ReadyToShoot = true;
}

