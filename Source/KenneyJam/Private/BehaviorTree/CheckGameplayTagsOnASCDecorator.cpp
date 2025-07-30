// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/CheckGameplayTagsOnASCDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"

UCheckGameplayTagsOnASCDecorator::UCheckGameplayTagsOnASCDecorator()
{
	NodeName = "Check Gameplay Tags on ASC";
    TagsToMatch = EGameplayContainerMatchType::Any;
}
bool UCheckGameplayTagsOnASCDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("Blackboard component not found!"));
        return false;
    }

   
    UObject* TargetObject = BlackboardComp->GetValueAsObject(ActorToCheck.SelectedKeyName);
    
    if (!TargetObject)
    {
        UE_LOG(LogTemp, Error, TEXT("Blackboard key [%s] is NULL!"), *ActorToCheck.SelectedKeyName.ToString());
        return false;
    }

    AActor* TargetActor = Cast<AActor>(TargetObject);
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Blackboard object is not a valid AActor!"));
        return false;
    }
    
    IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(TargetActor);
    if (!AbilityInterface)
    {
        UE_LOG(LogTemp, Error, TEXT("Actor %s does not implement IAbilitySystemInterface"), *TargetActor->GetName());
        return false;
    }

    UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent not found on %s"), *TargetActor->GetName());
        return false;
    }

    
    FGameplayTagContainer OwnedTags;
    ASC->GetOwnedGameplayTags(OwnedTags);
    
    
    switch (TagsToMatch)
    {
        case EGameplayContainerMatchType::All:
            return OwnedTags.HasAll(GameplayTags);

        case EGameplayContainerMatchType::Any:
            return OwnedTags.HasAny(GameplayTags);

        default:
            UE_LOG(LogBehaviorTree, Warning, TEXT("Invalid TagsToMatch value."));
            return false;
    }
}