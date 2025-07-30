// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthUpdated, float, NewValue,float,OldValue);
UCLASS()
class KENNEYJAM_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnHealthUpdated OnHealthUpdated;
	
	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,Health);

	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData PlayerSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,PlayerSpeed);

	UPROPERTY(BlueprintReadOnly,Category="Throwing")
	FGameplayAttributeData WeaponRange;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,WeaponRange);

	UPROPERTY(BlueprintReadOnly,Category="Throwing")
	FGameplayAttributeData MaxChargeSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,MaxChargeSpeed);

	UPROPERTY(BlueprintReadOnly,Category="Throwing")
	FGameplayAttributeData ThrowingWeaponForce;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,ThrowingWeaponForce);
	
	UPROPERTY(BlueprintReadOnly,Category="Throwing")
	FGameplayAttributeData ThrowingWeaponSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,ThrowingWeaponSpeed);

	UPROPERTY(BlueprintReadOnly,Category="Throwing")
	FGameplayAttributeData RetrievingWeaponSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet,RetrievingWeaponSpeed);

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
