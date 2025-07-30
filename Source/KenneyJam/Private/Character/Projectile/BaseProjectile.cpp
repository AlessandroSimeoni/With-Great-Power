// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Projectile/BaseProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/Projectile/Muzzle.h"
#include "Utils/EffectData.h"

#define COLLISION_PROJECTILE ECC_GameTraceChannel5
ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	RootComponent = NiagaraComponent;
}

void ABaseProjectile::Shoot(const UMuzzle* InstigatorMuzzle, const float ProjectileSpeed, const AActor* ProjectileTargetActor)
{
	// ignore collision with the owner of the muzzle
	CollisionParams.AddIgnoredActor(InstigatorMuzzle->GetOwner());
	
	FTimerManager& TM = GetWorld()->GetTimerManager();
	TM.SetTimer(CollisionTimeoutTH, this, &ThisClass::EndProjectileLifeCycle, CollisionTimeout, false);
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionParams.AddIgnoredActor(this);
}

void ABaseProjectile::HandleCollisionEffectOver(UNiagaraComponent* PSystem)
{
	EndProjectileLifeCycle();
}

void ABaseProjectile::HandleCollision()
{
	FTimerManager& TM = GetWorld()->GetTimerManager();
	TM.ClearTimer(CollisionTimeoutTH);
	
	CanShoot = false;
	
	AActor* HitActor = TraceHit.GetActor();
	if (HitActor)
	{
		
		static const FGameplayTag HitEventTag = FGameplayTag::RequestGameplayTag(FName("Character.ReceiveDamage"));
		
		FGameplayEventData EventData;
		EventData.EventTag = HitEventTag;
		EventData.Instigator = GetOwner();
		EventData.Target = HitActor;
		EventData.OptionalObject = nullptr;
		EventData.OptionalObject2 = nullptr;
		EventData.ContextHandle = FGameplayEffectContextHandle();
		EventData.InstigatorTags = FGameplayTagContainer();
		EventData.TargetTags = FGameplayTagContainer();
		EventData.EventMagnitude = Damage;
		
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (TargetASC)
		{
			TargetASC->HandleGameplayEvent(HitEventTag, &EventData);
		}
	}

	CollisionFX.SpawnEffect(GetWorld(),GetActorLocation());
	EndProjectileLifeCycle();
}

void ABaseProjectile::EndProjectileLifeCycle()
{
	OnProjectileEndLifeCycle.Broadcast();
	Destroy();
}

bool ABaseProjectile::SphereDetect()
{
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);
	const FVector SphereLocation = GetActorLocation() +
							 GetActorForwardVector() * SphereStartOffset.X +
							 GetActorUpVector() * SphereStartOffset.Z +
							 GetActorRightVector() * SphereStartOffset.Y;

	const bool Hit = GetWorld()->SweepSingleByChannel(
	   TraceHit, 
	   SphereLocation, 
	   SphereLocation + GetActorForwardVector() * SphereDistance, 
	   GetActorRotation().Quaternion(), 
	   ECC_Camera, 
	   Sphere,
	   CollisionParams
	);

		
	if (SphereDebug)
	{

		DrawDebugSphere(GetWorld(), SphereLocation, SphereRadius, 12, FColor::Red, false, 0.05f);
		DrawDebugSphere(GetWorld(), SphereLocation + GetActorForwardVector() * SphereDistance, SphereRadius, 12, FColor::Red, false, 0.05f);
	}

	return Hit;
}

bool ABaseProjectile::LineDetect()
{
	const FVector LineLocation = GetActorLocation() +
								 GetActorForwardVector() * LineStartOffset.X +
								 GetActorUpVector() * LineStartOffset.Z +
								 GetActorRightVector() * LineStartOffset.Y;

	const bool Hit = GetWorld()->LineTraceSingleByChannel(TraceHit,
	                                                      LineLocation,
	                                                      LineLocation + GetActorForwardVector() * LineDistance,
	                                                      COLLISION_PROJECTILE,
	                                                      CollisionParams);

	if (LineDebug)
		DrawDebugLine(GetWorld(), LineLocation, LineLocation + GetActorForwardVector() * LineDistance, FColor::Red, false, 0.01f);
	
	return Hit;
}
