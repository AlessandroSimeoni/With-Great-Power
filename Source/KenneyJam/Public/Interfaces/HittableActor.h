// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HittableActor.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHittableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KENNEYJAM_API IHittableActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "My Interface")
	bool Hit(float DamageValue, AActor* AttackInstigator);
};
