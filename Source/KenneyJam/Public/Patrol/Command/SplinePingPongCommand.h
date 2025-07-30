// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Patrol/Interface/SplineBehaviorCommand.h"
#include "UObject/Object.h"
#include "SplinePingPongCommand.generated.h"

/**
 * 
 */
UCLASS()
class KENNEYJAM_API USplinePingPongCommand : public UObject, public  ISplineBehaviorCommand
{
	GENERATED_BODY()
public:
	virtual FVector Execute(USplineComponent* Spline, int32& Index) override;
	virtual void IncrementIndex(class USplineComponent*  Spline, int32& Index) override;	
private:
	int8 Direction = 1;;
};
