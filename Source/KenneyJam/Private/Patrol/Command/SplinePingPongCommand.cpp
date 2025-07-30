// Fill out your copyright notice in the Description page of Project Settings.


#include "Patrol/Command/SplinePingPongCommand.h"

#include "Components/SplineComponent.h"

FVector USplinePingPongCommand::Execute(USplineComponent* Spline, int32& Index)
{
	FVector WorldPosition = Spline->GetLocationAtSplinePoint(Index,ESplineCoordinateSpace::Local);
	return WorldPosition;
}

void USplinePingPongCommand::IncrementIndex(class USplineComponent* Spline, int32& Index)
{
	if (Spline->GetNumberOfSplinePoints() == 1)
		return;
	
	Index = FMath::Max(0,Index + Direction);
	
	if (Index >= Spline->GetNumberOfSplinePoints() -1 || Index == 0)
		Direction = -Direction;
}
