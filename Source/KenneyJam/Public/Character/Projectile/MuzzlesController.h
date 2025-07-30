// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"
#include "Components/ActorComponent.h"
#include "MuzzlesController.generated.h"


class UMuzzle;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KENNEYJAM_API UMuzzlesController : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TArray<UMuzzle*> Muzzles;

public:
	UFUNCTION(BlueprintCallable)
	TArray<UMuzzle*> GetAvailableMuzzles();

	UFUNCTION(BlueprintCallable)
	ABaseProjectile*  ShootWithMuzzle(int32 MuzzleId, TSubclassOf<ABaseProjectile> ProjectileClass, float ProjectileSpeed, const AActor*
	                     ProjectileTargetActor, float Damage, float Destruction, const float KnockbackIntensity);

	

};
