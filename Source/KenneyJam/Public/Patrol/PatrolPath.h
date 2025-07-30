// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class KENNEYJAM_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPath();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Spline")
	TObjectPtr<USplineComponent> Spline;
	
};
