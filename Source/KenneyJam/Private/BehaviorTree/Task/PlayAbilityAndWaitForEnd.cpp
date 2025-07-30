// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/PlayAbilityAndWaitForEnd.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

uint16 UPlayAbilityAndWaitForEnd::GetInstanceMemorySize() const
{
	return sizeof(FPlayAbilityAndWaitForEndMemory);
}

EBTNodeResult::Type UPlayAbilityAndWaitForEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
 
	AActor* OwnerActor = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!OwnerActor)
	{
		return EBTNodeResult::Failed;
	}

	
	TScriptInterface<IAbilitySystemInterface> ASCInterface = OwnerActor;
	if (!ASCInterface)
	{
		return EBTNodeResult::Failed;
	}
	
	AbilitySystemComponent = ASCInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return EBTNodeResult::Failed;
	}
	
	if (!AbilitySystemComponent->TryActivateAbilityByClass(Ability))
	{
		return EBTNodeResult::Failed;
	}

	FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);
	if (!AbilitySpec)
	{
		return EBTNodeResult::Failed;
	}

	FPlayAbilityAndWaitForEndMemory* MyMemory = (FPlayAbilityAndWaitForEndMemory*)NodeMemory;
	MyMemory->Handle = AbilitySpec->Handle;
	MyMemory->AbilityEndedDelegateHandle = AbilitySystemComponent->OnAbilityEnded.AddUObject(
		this, &UPlayAbilityAndWaitForEnd::AbilityEnded, &OwnerComp
	);

	return EBTNodeResult::InProgress;
}

void UPlayAbilityAndWaitForEnd::AbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	uint8* NodeMemory = OwnerComp->GetNodeMemory(this, OwnerComp->FindInstanceContainingNode(this));
	if (!NodeMemory) return;

	FPlayAbilityAndWaitForEndMemory* MyMemory = (FPlayAbilityAndWaitForEndMemory*)NodeMemory;

	if (AbilityEndedData.AbilitySpecHandle == MyMemory->Handle)
	{
		if (IsValid(AbilitySystemComponent))
		{
			AbilitySystemComponent->OnAbilityEnded.Remove(MyMemory->AbilityEndedDelegateHandle);
		}

		OwnerComp->OnTaskFinished(this, EBTNodeResult::Succeeded);
		UE_LOG(LogTemp, Display, TEXT("Ability Ended (PlayAbilityAndWaitForEnd)"));
	}
}

EBTNodeResult::Type UPlayAbilityAndWaitForEnd::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FPlayAbilityAndWaitForEndMemory* MyMemory = (FPlayAbilityAndWaitForEndMemory*)NodeMemory;

	UE_LOG(LogTemp, Warning, TEXT("PlayAbilityAndWaitForEnd Aborted!"));

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->OnAbilityEnded.Remove(MyMemory->AbilityEndedDelegateHandle);

		if (CancelAbilityOnAbort && MyMemory->Handle.IsValid())
		{
			AbilitySystemComponent->CancelAbilityHandle(MyMemory->Handle);
			UE_LOG(LogTemp, Warning, TEXT("Ability Cancelled due to Abort."));
		}
	}

	return EBTNodeResult::Aborted;
}