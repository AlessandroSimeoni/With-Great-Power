// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemies/Enemy.h"
#include "AbilitySystemComponent.h"
#include "Character/Enemies/EnemyController.h"
#include "Character/GAS/EnemyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	PatrolSplineComponent = CreateDefaultSubobject<UPatrolSplineComponent>("PatrolSplineComponent");
}

void AEnemy::BeginPlay()
{

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
		BasicsAttributesSet = AbilitySystemComponent->GetSet<UEnemyAttributeSet>();
		if (BasicsAttributesSet)
		{
		GetCharacterMovement()->MaxWalkSpeed = BasicsAttributesSet->GetSpeed();
		Cast<AEnemyController>(GetController())->SetDetection(BasicsAttributesSet->GetViewRange(),BasicsAttributesSet->GetMinViewRange(),BasicsAttributesSet->GetViewAngle());
		}
	}
	
	GiveDefaultAbility();
	Super::BeginPlay();

	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AEnemy::EnemyDeath(AActor* ActorInstigator)
{
	OnDeath.Broadcast(this, ActorInstigator);
}

void AEnemy::EnemyRevive()
{
	OnRevive.Broadcast(this);
}

void AEnemy::SwitchMovementMode(bool bUseDesiredRotationMode)
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		if (bUseDesiredRotationMode)
		{
			MovementComp->bUseControllerDesiredRotation = true;
			MovementComp->bOrientRotationToMovement = false;
		}
		else
		{
			MovementComp->bUseControllerDesiredRotation = false;
			MovementComp->bOrientRotationToMovement = true;
		}
	}
}


UPatrolSplineComponent* AEnemy::GetPatrolSplineComponent_Implementation()
{
	return PatrolSplineComponent;
}

void AEnemy::GiveDefaultAbility()
{
	if (!IsValid(AbilitySystemComponent))return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbility)
	{
		const FGameplayAbilitySpec AbilitySpec(AbilityClass);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}
