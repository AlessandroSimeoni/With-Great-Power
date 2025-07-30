// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/JamGameMode.h"
#include "Character/Enemies/Enemy.h"
#include "Choas/Destructible.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AJamGameMode::BeginPlay()
{
	Super::BeginPlay();

	
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	if (Controller)
	{
		Controller->SetInputMode(InputMode);
		Controller->SetShowMouseCursor(true);
	}

	InitializeGameElements();
}

void AJamGameMode::InitializeGameElements()
{
	TArray<AActor*> Enemies;
	TArray<AActor*> Destructibles;

	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	EnemyCount = Enemies.Num();
	TotalEnemy = static_cast<float>(EnemyCount);

	for (AActor* EnemyActor : Enemies)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(EnemyActor))
		{
			Enemy->OnDeath.AddDynamic(this, &AJamGameMode::HandleEnemyDeathGameMode);
			Enemy->OnRevive.AddDynamic(this, &AJamGameMode::HandleEnemyRevive);
		}
	}

	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADestructible::StaticClass(), Destructibles);
	DestructibleCount = Destructibles.Num();
	TotalDestruction = 0.0f;

	for (AActor* DestructibleActor : Destructibles)
	{
		if (ADestructible* Destructible = Cast<ADestructible>(DestructibleActor))
		{
			Destructible->OnDestroyedCollection.AddDynamic(this, &AJamGameMode::HandleBuildingDestroyed);
			TotalDestruction += static_cast<float>(Destructible->DestructionLevel);
		}
	}

	OnInitialization(TotalEnemy, TotalDestruction);
}

void AJamGameMode::HandleEnemyDeathGameMode(AActor* Enemy, const AActor* InstigatorActor)
{
	TotalEnemy = FMath::Max(0.0f, TotalEnemy - 1.0f);
	OnEnemyDied(Enemy, TotalEnemy);
}
void AJamGameMode::HandleEnemyRevive(AActor* Enemy)
{
	TotalEnemy += 1.0f;
	OnEnemyRevived(Enemy, TotalEnemy);
}

void AJamGameMode::HandleBuildingDestroyed(AActor* Actor, int32 DestructionLevel)
{
	TotalDestruction = FMath::Max(0.0f, TotalDestruction - static_cast<float>(DestructionLevel));
	OnBuildingDestroyed(Actor, TotalDestruction);
}

void AJamGameMode::InitializeEnemy(AEnemy* Enemy)
{
	Enemy->OnDeath.AddDynamic(this, &AJamGameMode::HandleEnemyDeathGameMode);
	Enemy->OnRevive.AddDynamic(this, &AJamGameMode::HandleEnemyRevive);
	HandleEnemyRevive(Enemy);
}
