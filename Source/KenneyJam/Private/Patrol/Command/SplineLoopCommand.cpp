// Fill out your copyright notice in the Description page of Project Settings.


#include "Patrol/Command/SplineLoopCommand.h"

#include "Components/SplineComponent.h"

FVector USplineLoopCommand::Execute(USplineComponent* Spline, int32& Index)
{
	FVector WorldPosition = Spline->GetLocationAtSplinePoint(Index,ESplineCoordinateSpace::Local);
	return WorldPosition;
}

void USplineLoopCommand::IncrementIndex(class USplineComponent*  Spline, int32& Index)
{
	if (Index >= Spline->GetNumberOfSplinePoints()-1)
		Index = 0;
	
	else Index++;
	
}
