// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemies/TaskRotateCharacter.h"
#include "AbilitySystemComponent.h"

UTaskRotateCharacter::UTaskRotateCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TotalTime = 0.0f;
	ElapsedTime = 0.0f;
	bTickingTask = true;
}

UTaskRotateCharacter* UTaskRotateCharacter::RotateCharacter(UGameplayAbility* OwningAbility, FName TaskInstanceName, float RotationDegrees, float Duration, UCurveFloat* RotationCurve)
{
	UTaskRotateCharacter* MyTask = NewAbilityTask<UTaskRotateCharacter>(OwningAbility, TaskInstanceName);
	if (MyTask)
	{
		MyTask->TotalTime = Duration;
		MyTask->ElapsedTime = 0.0f;
		MyTask->RotationCurve = RotationCurve;

		AActor* OwnerActor = OwningAbility->GetAvatarActorFromActorInfo();
		if (OwnerActor)
		{
			MyTask->OwnerActor = OwnerActor;
			MyTask->StartRotation = OwnerActor->GetActorRotation();
			MyTask->TargetRotation = MyTask->StartRotation + FRotator(0.0f, RotationDegrees, 0.0f);
		}
		MyTask->ReadyForActivation();
	}
	return MyTask;
}


void UTaskRotateCharacter::Activate()
{
	if (OwnerActor)
	{
		SetWaitingOnAvatar();
		
	}
}

void UTaskRotateCharacter::TickTask(float DeltaTime)
{
	UE_LOG(LogTemp, Display, TEXT("Tick Task - Elapsed: %f / %f"), ElapsedTime, TotalTime);

	if (!OwnerActor || ElapsedTime >= TotalTime)
	{
		if (ElapsedTime >= TotalTime)
		{
			if (OwnerActor)
			{
				OwnerActor->SetActorRotation(TargetRotation);
			}
			OnRotationCompleted.Broadcast();
		}
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	ElapsedTime += DeltaTime;
	float Alpha = ElapsedTime / TotalTime;

	
	float CurveAlpha = RotationCurve ? RotationCurve->GetFloatValue(Alpha) : Alpha;

	FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, CurveAlpha);
	OwnerActor->SetActorRotation(NewRotation);
}


void UTaskRotateCharacter::OnDestroy(bool AbilityEnded)
{
	if (AbilityEnded)
	{
		OnFinished.Broadcast();
	}
	Super::OnDestroy(AbilityEnded);
}