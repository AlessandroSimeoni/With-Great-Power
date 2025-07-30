// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TaskRotateCharacter.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotationCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskFinished);
UCLASS()
class KENNEYJAM_API UTaskRotateCharacter : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnRotationCompleted OnRotationCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnTaskFinished OnFinished;

	 UTaskRotateCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Rotate Character", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UTaskRotateCharacter* RotateCharacter(UGameplayAbility* OwningAbility, FName TaskInstanceName, float RotationDegrees, float Duration, UCurveFloat* RotationCurve = nullptr);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;


protected:
	virtual void OnDestroy(bool AbilityEnded) override;

private:
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	UPROPERTY()
	TObjectPtr<UCurveFloat> RotationCurve;
	FRotator StartRotation;
	FRotator TargetRotation;
	float TotalTime;
	float ElapsedTime;
	
};
