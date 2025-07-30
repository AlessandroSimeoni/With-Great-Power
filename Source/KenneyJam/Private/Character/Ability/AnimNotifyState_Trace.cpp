#include "Character/Ability/AnimNotifyState_Trace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"

UAnimNotifyState_Trace::UAnimNotifyState_Trace()
{
	FRichCurve* RadiusCurve = TraceRadius.GetRichCurve();
	if (RadiusCurve)
	{
		RadiusCurve->AddKey(0.0f, 50.0f); 
		RadiusCurve->AddKey(1.0f, 50.0f);
	}

	FRichCurve* DistanceCurve = TraceDistance.GetRichCurve();
	if (DistanceCurve)
	{
		DistanceCurve->AddKey(0.0f, 100.0f); 
		DistanceCurve->AddKey(1.0f, 100.0f);
	}

	FRichCurve* YawCurve = TraceYaw.GetRichCurve();
	if (YawCurve)
	{
		YawCurve->AddKey(0.0f, 0.0f);
		YawCurve->AddKey(1.0f, 0.0f);
	}

	FRichCurve* PitchCurve = TracePitch.GetRichCurve();
	if (PitchCurve)
	{
		PitchCurve->AddKey(0.0f, 0.0f);
		PitchCurve->AddKey(1.0f, 0.0f);
	}
}

void UAnimNotifyState_Trace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	const FAnimNotifyEvent* NotifyEvent = EventReference.GetNotify();
	if (!NotifyEvent)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	HitActorsAndComponents.Empty();

	FInstanceProgressInfo ProgressInfo;
	ProgressInfo.LastTraceTime = 0.f;
	ProgressInfo.CurrentTotalDuration = TotalDuration;
	ProgressInfo.ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	ProgressInfo.MontageInstance = AnimInstance->GetActiveInstanceForMontage(NotifyEvent->GetLinkedMontage());

	ProgressInfoMap.Add(MeshComp->GetUniqueID(), MoveTemp(ProgressInfo));
}

void UAnimNotifyState_Trace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	FInstanceProgressInfo* Info = ProgressInfoMap.Find(MeshComp->GetUniqueID());
	if (!Info)
	{
		return;
	}

	if (!Info->MontageInstance)
	{
		return;
	}

	const FAnimNotifyEvent* NotifyEvent = EventReference.GetNotify();
	if (!NotifyEvent)
	{
		return;
	}

	float CurrentTime = MeshComp->GetWorld()->GetTimeSeconds();
	float MontageCurrentTime = Info->MontageInstance->GetPosition() - NotifyEvent->GetTriggerTime();
	float Alpha = FMath::Clamp(MontageCurrentTime / Info->CurrentTotalDuration, 0.f, 1.f);

	float FrameInterval = 1.0f / TargetFrameRate;

	if (CurrentTime - Info->LastTraceTime >= FrameInterval)
	{
		PerformTrace(MeshComp, Alpha, *Info);
		Info->LastTraceTime = CurrentTime;
	}
}

void UAnimNotifyState_Trace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (ProgressInfoMap.Contains(MeshComp->GetUniqueID()))
	{
		FInstanceProgressInfo ProgressInfo = ProgressInfoMap.FindAndRemoveChecked(MeshComp->GetUniqueID());
		PerformTrace(MeshComp, 1.f, ProgressInfo);
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UAnimNotifyState_Trace::PerformTrace(const USkeletalMeshComponent* MeshComp, float Alpha, FInstanceProgressInfo& ProgressInfo)
{
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	FVector TraceStartPosition;
	if (MeshComp->DoesSocketExist(StartSocket))
	{
		TraceStartPosition = MeshComp->GetSocketLocation(StartSocket);
	}
	else
	{
		TraceStartPosition = MeshComp->GetComponentTransform().TransformPosition(TraceStartLocal);
	}

	FRotator TraceRotation = MeshComp->GetComponentRotation();
	if (IsControlRotationSpace)
	{
		if (APawn* OwningPawn = Cast<APawn>(Owner))
		{
			TraceRotation = OwningPawn->GetControlRotation();
		}
	}

	TraceRotation.Yaw += TraceYaw.GetRichCurveConst()->Eval(Alpha);
	TraceRotation.Pitch += TracePitch.GetRichCurveConst()->Eval(Alpha);

	FVector Direction = TraceRotation.Vector();
	FVector End;

	if (MeshComp->DoesSocketExist(EndSocket))
	{
		End = MeshComp->GetSocketLocation(EndSocket);
	}
	else
	{
		End = TraceStartPosition + (Direction * TraceDistance.GetRichCurveConst()->Eval(Alpha));
	}

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	bool bHit = false;

	if (IsSphereTrace)
	{
		bHit = UKismetSystemLibrary::SphereTraceSingle(
			MeshComp->GetWorld(), TraceStartPosition, End,
			TraceRadius.GetRichCurveConst()->Eval(Alpha),
			TraceChannel, false, { Owner }, DebugType, HitResult, true
		);
	}
	else
	{
		bHit = UKismetSystemLibrary::LineTraceSingle(
			MeshComp->GetWorld(), TraceStartPosition, End,
			TraceChannel, false, { Owner }, DebugType, HitResult, true
		);
	}

	if (bHit && HitResult.GetActor() && HitResult.Component.IsValid())
	{
		AActor* HitActor = HitResult.GetActor();
		UPrimitiveComponent* HitComponent = HitResult.Component.Get();

		if (!ProgressInfo.HitActorsAndComponents.Contains(HitActor) ||
			!ProgressInfo.HitActorsAndComponents[HitActor].Contains(HitComponent))
		{
			ProgressInfo.HitActorsAndComponents.FindOrAdd(HitActor).Add(HitComponent);

			if (ProgressInfo.ASC.IsValid())
			{
				FGameplayEventData EventData;
				EventData.EventTag = HitEventTag;
				EventData.Instigator = Owner;
				EventData.Target = HitActor;
				EventData.OptionalObject = HitComponent;
				EventData.ContextHandle = ProgressInfo.ASC->MakeEffectContext();
				EventData.ContextHandle.AddOrigin(HitResult.ImpactPoint);
				ProgressInfo.ASC->HandleGameplayEvent(HitEventTag, &EventData);
			}
		}
	}
}
