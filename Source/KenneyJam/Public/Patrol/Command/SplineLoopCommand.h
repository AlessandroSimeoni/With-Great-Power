// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Patrol/Interface/SplineBehaviorCommand.h"
#include "SplineLoopCommand.generated.h"

/**
 * 
 */
UCLASS()
class KENNEYJAM_API USplineLoopCommand : public UObject,  public ISplineBehaviorCommand
{
	GENERATED_BODY()
	virtual FVector Execute(USplineComponent* Spline, int32& Index) override;
	virtual void IncrementIndex(class USplineComponent*  Spline, int32& Index) override;	
};
