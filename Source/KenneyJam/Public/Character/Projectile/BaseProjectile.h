// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "Utils/EffectData.h"
#include "BaseProjectile.generated.h"

class UMuzzle;
class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjectileEndLifeCycle);
UCLASS(Abstract)
class KENNEYJAM_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABaseProjectile();

	UPROPERTY(BlueprintAssignable)
	FOnProjectileEndLifeCycle OnProjectileEndLifeCycle;
	
	UFUNCTION(BlueprintCallable, Category="Projectile")
	virtual void Shoot(const UMuzzle* InstigatorMuzzle, const float ProjectileSpeed, const AActor* ProjectileTargetActor = nullptr);

	void SetDamage(const float DamageValue){if (!DamageDestructionOverridable) return; Damage = DamageValue;}
	void SetDestruction(const float DestructionValue){if (!DamageDestructionOverridable) return; DestructionLevel = DestructionValue;}
	void SetKnockbackIntensity(const float KnockbackIntensity){if (!DamageDestructionOverridable) return; KnockbackIntensityParameter = KnockbackIntensity;}
	
protected:
	bool CanShoot = false;
	float Speed = 0.0f;
	FHitResult TraceHit;
	FCollisionQueryParams CollisionParams;
	FTimerHandle CollisionTimeoutTH;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FEffectData CollisionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	bool DamageDestructionOverridable = true;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", Meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float Damage = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DestructionLevel = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float KnockbackIntensityParameter = 1.0f;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", Meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float CollisionTimeout = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SphereTrace Detection", Meta = (UIMin = 0.01f, ClampMin = 0.01f))
	float SphereRadius = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SphereTrace Detection")
	FVector SphereStartOffset = FVector::Zero();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SphereTrace Detection", Meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float SphereDistance = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SphereTrace Detection")
	bool SphereDebug = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LineTrace Detection")
	FVector LineStartOffset = FVector::Zero();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LineTrace Detection", Meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float LineDistance = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LineTrace Detection")
	bool LineDebug = false;

	UFUNCTION()
	virtual void HandleCollisionEffectOver(UNiagaraComponent* PSystem);

	virtual void BeginPlay() override;
	virtual void HandleCollision();
	virtual void EndProjectileLifeCycle();
	bool SphereDetect();
	bool LineDetect();
};
