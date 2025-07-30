// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EnemyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 


UCLASS()
class KENNEYJAM_API UEnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,Attack)
	
	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData ViewRange;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,ViewRange)

	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData ViewAngle;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,ViewAngle);

	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData MinViewRange;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,MinViewRange);
	
	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,Speed);

	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData DownDuration;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,DownDuration);

	UPROPERTY(BlueprintReadOnly,Category="Base Stats")
	FGameplayAttributeData ReviveDelay;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet,ReviveDelay);

	
	void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue);;
	
};
