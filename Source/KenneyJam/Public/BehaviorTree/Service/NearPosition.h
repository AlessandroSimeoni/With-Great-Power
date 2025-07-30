// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BehaviorTree/BTService.h"
#include "Patrol/PatrolSplineComponent.h"
#include "NearPosition.generated.h"

struct FNearPositionMemory
{
	UPatrolSplineComponent* PatrolSplineComponent = nullptr;
};

UCLASS()
class KENNEYJAM_API UNearPosition : public UBTService
{
	GENERATED_BODY()

protected:
	UNearPosition();
	/**Function*/
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FNearPositionMemory); } 
	/**Property*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UPatrolSplineComponent> PatrolSplineComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FBlackboardKeySelector IsPatrolPathAssigned;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FBlackboardKeySelector Position;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MinDistance = 0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool DrawDebug = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ability")
	TSubclassOf<UGameplayAbility> AbilityTurn;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ability")
	float TurnAngleTollerance = 15;
	void DrawDebugPatrol(FVector NewPosition);
private:
	
	/** check if the new position is behind the pawn and need to tunr quickly*/
	bool IsPositionBehind(APawn* ControlledPawn, FVector NewPosition);
};

