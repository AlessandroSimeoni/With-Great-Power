// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainCharacter.h"

#include "AbilitySystemComponent.h"
#include "Character/GAS/PlayerAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AbilitySystemComponent))
		AbilitySystemComponent->InitAbilityActorInfo(this,this);

	GiveDefaultAbility();
	AttributeSet = AbilitySystemComponent->GetSet<UPlayerAttributeSet>();

	GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetPlayerSpeed();
}

void AMainCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MovementDirection == FVector::ZeroVector)
	{
		return;
	}
	
	ConsumeMovement();
}

void AMainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
	}

	SetOwner(NewController);
}

void AMainCharacter::GiveDefaultAbility()
{
	if (!IsValid(AbilitySystemComponent))return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbility)
	{
		//Create a rappresentation of the ability
		const FGameplayAbilitySpec AbilitySpec(AbilityClass);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

void AMainCharacter::ConsumeMovement()
{
	if (!AttributeSet)
	{
		return; 
	}
	
	AddMovementInput(MovementDirection * AttributeSet->GetPlayerSpeed());
	MovementDirection = FVector::ZeroVector;
}

void AMainCharacter::AddMovementDirection(FVector Direction)
{
	MovementDirection += Direction;
}

void AMainCharacter::BeginThrowAction()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(BeginThrowActionTag));
}

void AMainCharacter::ThrowWeapon()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(ThrowWeaponTag));
}

void AMainCharacter::RetrieveWeapon()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(RetrieveWeaponTag));
}

void AMainCharacter::TeleportToWeapon()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(TeleportToWeaponTag));
}

UAbilitySystemComponent* AMainCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AMainCharacter::Hit_Implementation(float DamageValue, AActor* AttackInstigator)
{
	FGameplayEventData Payload;
	Payload.EventTag = ReceiveDamageTag;
	Payload.Instigator = AttackInstigator;
	Payload.Target = this;
	Payload.EventMagnitude = DamageValue;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Payload.EventTag, Payload);
	
	return true;
}

