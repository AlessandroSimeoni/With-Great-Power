// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SplineBehaviorCommand.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USplineBehaviorCommand : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KENNEYJAM_API ISplineBehaviorCommand
{
	GENERATED_BODY()
public:
	virtual FVector Execute(class USplineComponent*  Spline, int32& Index) = 0;
	virtual void IncrementIndex(class USplineComponent*  Spline, int32& Index) = 0;
};
