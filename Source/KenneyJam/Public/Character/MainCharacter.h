// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interfaces/HittableActor.h"
#include "MainCharacter.generated.h"

class UPlayerAttributeSet;
class UGameplayAbility;
class UAbilitySystemComponent;

UCLASS()
class KENNEYJAM_API AMainCharacter : public ACharacter, public IAbilitySystemInterface, public IHittableActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbility;
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag ReceiveDamageTag;
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag BeginThrowActionTag;
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag ThrowWeaponTag;
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag RetrieveWeaponTag;
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	FGameplayTag TeleportToWeaponTag;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	void GiveDefaultAbility();
	void ConsumeMovement();
	
	FVector MovementDirection = FVector::ZeroVector;
	UPROPERTY()
	const UPlayerAttributeSet* AttributeSet;
	
public:
	AMainCharacter();

	UFUNCTION(BlueprintCallable, Category="Locomotion")
	void AddMovementDirection(FVector Direction);
	void BeginThrowAction();
	void ThrowWeapon();
	void RetrieveWeapon();
	void TeleportToWeapon();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual bool Hit_Implementation(float DamageValue, AActor* AttackInstigator) override;
};
