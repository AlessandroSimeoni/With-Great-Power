// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Patrol/Interface/PatrolInterface.h"
#include "Enemy.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UEnemyAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, AActor*, Actor, const AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRevive, AActor*, Actor);
UCLASS()
class KENNEYJAM_API AEnemy : public ACharacter,public IAbilitySystemInterface, public IPatrolInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	UPROPERTY(BlueprintAssignable, Category="Creature")
	FOnDeath OnDeath;
	UPROPERTY(BlueprintAssignable, Category="Creature")
	FOnRevive OnRevive;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	void EnemyDeath(AActor* ActorInstigato);
	UFUNCTION(BlueprintCallable)
	void EnemyRevive();
	UFUNCTION(BlueprintCallable)
	void SwitchMovementMode(bool bUseDesiredRotationMode);
	UFUNCTION(BlueprintCallable,Category="Patrol")
	virtual UPatrolSplineComponent* GetPatrolSplineComponent_Implementation() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GAS",meta=(AllowPrivateAccess=true))
	const class UEnemyAttributeSet* BasicsAttributesSet;

	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Patrol")
	TObjectPtr<UPatrolSplineComponent> PatrolSplineComponent;
	
	UPROPERTY(EditDefaultsOnly,Category="Gas")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbility;

	void GiveDefaultAbility();

	
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};
