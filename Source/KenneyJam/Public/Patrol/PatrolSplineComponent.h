// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatrolPath.h"
#include "Components/ActorComponent.h"
#include "Patrol/Interface/SplineBehaviorCommand.h"
#include "PatrolSplineComponent.generated.h"

struct FScriptedAction;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnScriptedAction, const FScriptedAction&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewPosition, int32, PreviousPosition, int32, CurrentPosition);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KENNEYJAM_API UPatrolSplineComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	FOnScriptedAction OnScriptedAction;
	
	UPROPERTY(BlueprintAssignable, Category="Spline")
	FOnNewPosition OnNewPosition;
	
	
	UPROPERTY(BlueprintReadWrite,Category="Spline")
	int32 CurrentIndex;
	
	UFUNCTION(BlueprintCallable,Category="Spline")
	void SetUpCommand();
	UFUNCTION(BlueprintCallable,Category="Spline")
	FVector GetNextPosition();
	UFUNCTION(BlueprintCallable,Category="Spline")
	FVector GetPosition();
	UFUNCTION(BlueprintCallable,Category="Spline")
	void IncrementIndex();
	
	UFUNCTION(BlueprintCallable,Category="Spline")
	FVector GetCurrentPosition();
	
	UFUNCTION(BlueprintCallable,Category="Spline")
	FVector IncrementAndGetCurrentPosition();

	UFUNCTION(BlueprintCallable,Category="Spline")
	bool IncrementIndexAndSendScriptedActionEvent();

	
	/**Return if the path is assigned*/
	UFUNCTION(BlueprintCallable,Category="Spline")
	bool IsPathAssigned();
	
/**Set spline  method they reset the Current Position to 0 */
	UFUNCTION(blueprintCallable,Category="Spline")
	void SetCurrentSplineActor(APatrolPath* PatrolPath);
	void SetCurrentSplineActor();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spline")
	TObjectPtr<APatrolPath> AssignedPatrolPath;
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> CommandReference;
	
	UPROPERTY()
	TScriptInterface<ISplineBehaviorCommand> Command;
	
		
};
