// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AbilitySystemComponent.h"
#include "WaitAbilityEnd.generated.h"

USTRUCT()
struct FWaitAbilityEndMemory
{
	GENERATED_BODY()

	FGameplayAbilitySpecHandle Handle;
	FDelegateHandle AbilityEndedDelegateHandle;
};

UCLASS()
class KENNEYJAM_API UWaitAbilityEnd : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	
	void AbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Ability")
	bool CancelAbilityOnAbort;
	
	FDelegateHandle AbilityEndedDelegateHandle;
	
	FGameplayAbilitySpecHandle Handle;
	
};
