// Fill out your copyright notice in the Description page of Project Settings.


#include "Choas/Destructible.h"

#include "AbilitySystemComponent.h"
#include "Character/Enemies/Enemy.h"

// Sets default values
ADestructible::ADestructible()
{
 
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ADestructible::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADestructible::TriggerDestruction()
{
	OnDestroyedCollection.Broadcast(this,DestructionLevel);
}

// Called every frame
void ADestructible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADestructible::SpawnEnemies(FVector CenterSpawnPosition)
{
	if (Enemies.Num() == 0) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FGameplayTag SpawnAbilityTag = FGameplayTag::RequestGameplayTag(FName("Action.Spawn"));
	FVector CenterLocation = CenterSpawnPosition;
	float Radius = Spawnradius; 

	for (auto EnemyClass : Enemies)
	{
		if (!EnemyClass) continue;

		FVector2D Random2D = FMath::RandPointInCircle(Radius);
		FVector SpawnLocation = CenterLocation + FVector(Random2D.X, Random2D.Y, 0.f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AEnemy* SpawnedEnemy = World->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		
		if (SpawnedEnemy)
		{
			SpawnedEnemy->SetActorHiddenInGame(true);

			UAbilitySystemComponent* ASC = SpawnedEnemy->FindComponentByClass<UAbilitySystemComponent>();
			if (ASC && SpawnAbilityTag.IsValid())
			{
				ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SpawnAbilityTag));
			}
		}
	}
}