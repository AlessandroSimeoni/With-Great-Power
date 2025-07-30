// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Destructible.generated.h"

class AEnemy;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDestroyedCollection, AActor*, actor, int32, DestructionLevel);
UCLASS()
class KENNEYJAM_API ADestructible : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestructible();

	UPROPERTY(BlueprintAssignable, Category="Destructible")
	FOnDestroyedCollection OnDestroyedCollection;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Chaos")
	int32  DestructionLevel = 0;
    

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Chaos")
	bool bBroken = false;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemies")
	TArray<TSubclassOf<AEnemy>> Enemies;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Enemies")
	float Spawnradius = 300.0f;

	UFUNCTION(BlueprintCallable, Category="Destructible")
	void TriggerDestruction();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category="Enemies")
	void SpawnEnemies(FVector CenterSpawnPosition);
};
