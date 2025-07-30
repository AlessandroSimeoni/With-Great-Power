#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JamGameMode.generated.h"

class AEnemy;

UCLASS()
class KENNEYJAM_API AJamGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Game")
	void InitializeGameElements();

	UFUNCTION()
	void HandleEnemyDeathGameMode(AActor* Enemy, const AActor* InstigatorActor);

	UFUNCTION()
	void HandleEnemyRevive(AActor* Enemy);

	UFUNCTION()
	void HandleBuildingDestroyed(AActor* Actor, int32 DestructionLevel);

	UFUNCTION(BlueprintCallable)
	void InitializeEnemy(AEnemy* Enemy);

	UPROPERTY(BlueprintReadWrite, Category="Game")
	int32 EnemyCount;

	UPROPERTY(BlueprintReadWrite, Category="Game")
	int32 DestructibleCount;

	UPROPERTY(BlueprintReadWrite, Category="Game")
	float TotalEnemy = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category="Game")
	float TotalDestruction = 0.0f;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Game")
	void OnEnemyDied(AActor* Enemy,float TotalEnemyNumber);

	UFUNCTION(BlueprintImplementableEvent, Category="Game")
	void OnEnemyRevived(AActor* Enemy,float TotalEnemyNumber);

	UFUNCTION(BlueprintImplementableEvent, Category="Game")
	void OnBuildingDestroyed(AActor* Building,float TotalDestructionNumber);

	UFUNCTION(BlueprintImplementableEvent, Category="Game")
	void OnInitialization(float TotalEnemyNumber ,float TotalDestructionNumber);
};
