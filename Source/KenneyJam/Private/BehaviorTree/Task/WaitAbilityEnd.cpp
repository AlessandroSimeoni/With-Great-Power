// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Task/WaitAbilityEnd.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

uint16 UWaitAbilityEnd::GetInstanceMemorySize() const
{
	return sizeof(FWaitAbilityEndMemory);
}

EBTNodeResult::Type UWaitAbilityEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	auto AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);
	if (!AbilitySpec || !AbilitySpec->IsActive())
	{
		return EBTNodeResult::Failed;
	}
	
	FWaitAbilityEndMemory* MyMemory = (FWaitAbilityEndMemory*)NodeMemory;
	MyMemory->Handle = AbilitySpec->Handle;
	MyMemory->AbilityEndedDelegateHandle = AbilitySystemComponent->OnAbilityEnded.AddUObject(
		this, &UWaitAbilityEnd::AbilityEnded, &OwnerComp
	);

	return EBTNodeResult::InProgress;

}

void UWaitAbilityEnd::AbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	uint8* NodeMemory = OwnerComp->GetNodeMemory(this, OwnerComp->FindInstanceContainingNode(this));
	if (!NodeMemory) return;

	FWaitAbilityEndMemory* MyMemory = (FWaitAbilityEndMemory*)NodeMemory;

	if (AbilityEndedData.AbilitySpecHandle == MyMemory->Handle)
	{
		if (IsValid(AbilitySystemComponent))
		{
			AbilitySystemComponent->OnAbilityEnded.Remove(MyMemory->AbilityEndedDelegateHandle);
		}

		OwnerComp->OnTaskFinished(this, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UWaitAbilityEnd::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FWaitAbilityEndMemory* MyMemory = (FWaitAbilityEndMemory*)NodeMemory;

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->OnAbilityEnded.Remove(MyMemory->AbilityEndedDelegateHandle);

		if (CancelAbilityOnAbort && MyMemory->Handle.IsValid())
		{
			AbilitySystemComponent->CancelAbilityHandle(MyMemory->Handle);
		}
	}

	return EBTNodeResult::Aborted;
}
