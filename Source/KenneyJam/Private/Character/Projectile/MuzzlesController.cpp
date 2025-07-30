// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Projectile/MuzzlesController.h"

#include "Character/Projectile/Muzzle.h"

void UMuzzlesController::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->GetComponents<UMuzzle>(Muzzles);
}

TArray<UMuzzle*> UMuzzlesController::GetAvailableMuzzles()
{
	if (Muzzles.Num() == 0)
		return TArray<UMuzzle*>();

	UE_LOG(LogTemp, Warning, TEXT("Muzzles exist"));
	TArray<UMuzzle*> Output;

	for (UMuzzle* Muzzle : Muzzles)
		if (Muzzle->IsReadyToShoot())
			Output.Add(Muzzle);
	
	return Output;
}

ABaseProjectile*  UMuzzlesController::ShootWithMuzzle(int32 MuzzleId, const TSubclassOf<ABaseProjectile> ProjectileClass, const float ProjectileSpeed, const AActor* ProjectileTargetActor,const float Damage, const float Destruction, const float KnockbackIntensity)
{
	for (UMuzzle* Muzzle : Muzzles)
	{
		if (MuzzleId == Muzzle->GetMuzzleID())
		{
			return Muzzle->Shoot(ProjectileClass, ProjectileSpeed, ProjectileTargetActor, Damage, Destruction, KnockbackIntensity);
		}
	}
	return nullptr;
}
