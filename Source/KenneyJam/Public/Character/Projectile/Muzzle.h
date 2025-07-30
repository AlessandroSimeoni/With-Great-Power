// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Muzzle.generated.h"


class ABaseProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KENNEYJAM_API UMuzzle : public USceneComponent
{
	GENERATED_BODY()

	
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Muzzle", Meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MuzzleCooldownTime = 0.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Muzzle")
	bool ReadyToShoot = true;
	
	
	UFUNCTION()
	void HandleCooldownOver();


public:
	UFUNCTION(BlueprintCallable, Category = "Muzzle")
	virtual ABaseProjectile*  Shoot(const TSubclassOf<ABaseProjectile> ProjectileClass, const float ProjectileSpeed, const AActor* ProjectileTargetActor = nullptr,const float Damage =0.5f, const float Destruction = 0, const float KnockbackIntensity = 1.0f, const float RandomAngle = 0.0f);
	UFUNCTION(BlueprintCallable, Category = "Muzzle")
	bool IsReadyToShoot();
	UFUNCTION(BlueprintCallable, Category = "Muzzle")
	int32 GetMuzzleID();
	
	
	FTimerHandle ShootCooldownTimerHandle;
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Muzzle");
	int32 MuzzleID;
		
};
