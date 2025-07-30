// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/Service/NearPosition.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Patrol/Interface/PatrolInterface.h"
#include "DrawDebugHelpers.h"

UNearPosition::UNearPosition()
{
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UNearPosition::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	FNearPositionMemory* MyMemory = (FNearPositionMemory*)NodeMemory;

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	IPatrolInterface* PatrolPawn = Cast<IPatrolInterface>(ControlledPawn);
	if (PatrolPawn)
	{
		MyMemory->PatrolSplineComponent = PatrolPawn->Execute_GetPatrolSplineComponent(ControlledPawn);
		if (!IsValid(MyMemory->PatrolSplineComponent)) return;
	}
}

void UNearPosition::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FNearPositionMemory* MyMemory = (FNearPositionMemory*)NodeMemory;

	if (!IsValid(MyMemory->PatrolSplineComponent)) return;

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	float PositionDistance = FVector::Distance(ControlledPawn->GetActorLocation(), 
												BlackboardComp->GetValueAsVector(Position.SelectedKeyName));

	if (PositionDistance < MinDistance && MyMemory->PatrolSplineComponent)
	{
		MyMemory->PatrolSplineComponent->IncrementIndex();
		FVector NewPosition = MyMemory->PatrolSplineComponent->GetPosition();
			if (NewPosition.Equals(FVector::ZeroVector))
			{
				BlackboardComp->SetValueAsBool(IsPatrolPathAssigned.SelectedKeyName, false);
				return;
			}
			
		BlackboardComp->SetValueAsVector(Position.SelectedKeyName, NewPosition);
		DrawDebugPatrol(NewPosition);

		/*
		UE_LOG(LogTemp, Warning, TEXT("New patrol position set for %s: %s"),
			*ControlledPawn->GetName(), *NewPosition.ToString());
		*/
		
		IAbilitySystemInterface* AbilitySystemPawn = Cast<IAbilitySystemInterface>(ControlledPawn);
		if (AbilitySystemPawn && IsValid(AbilityTurn))
		{
			if (IsPositionBehind(ControlledPawn, NewPosition))
			{
				TScriptInterface<IAbilitySystemInterface>(ControlledPawn)
					->GetAbilitySystemComponent()
					->TryActivateAbilityByClass(AbilityTurn);
			}
		}
	}
}
inline bool UNearPosition::IsPositionBehind(APawn* ControlledPawn, FVector NewPosition)
{
	if (!ControlledPawn) return false;

	FVector ForwardVector = ControlledPawn->GetActorForwardVector().GetSafeNormal();
	FVector DirectionToTarget = (NewPosition - ControlledPawn->GetActorLocation()).GetSafeNormal();
	
	float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	//UE_LOG(LogTemp, Warning, TEXT("Angle to Target: %f"), Angle);
	
	return Angle > 180 - TurnAngleTollerance/2 && Angle < 180 + TurnAngleTollerance/2;
}


void UNearPosition::DrawDebugPatrol(FVector NewPosition)
{
	if (!DrawDebug) return;
	
	DrawDebugSphere(
		  GetWorld(),
		  NewPosition,
		  50.0f,    
		  12,       
		  FColor::White, 
		  false,    
		  4.0f     
	  );
}
