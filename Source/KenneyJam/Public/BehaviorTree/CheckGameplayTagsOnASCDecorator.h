// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "CheckGameplayTagsOnASCDecorator.generated.h"

/**
 * 
 */
UCLASS()
class KENNEYJAM_API UCheckGameplayTagsOnASCDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	UCheckGameplayTagsOnASCDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActorToCheck;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Tags")
	FGameplayTagContainer GameplayTags;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Tags")
	EGameplayContainerMatchType TagsToMatch;
};
