#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AnimNotifyState_Trace.generated.h"

UCLASS()
class KENNEYJAM_API UAnimNotifyState_Trace : public UAnimNotifyState
{
	GENERATED_BODY()

protected:

	UAnimNotifyState_Trace();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FName StartSocket = "Start";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FName EndSocket = "End";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	bool IsControlRotationSpace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	bool IsSphereTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FGameplayTag HitEventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	float TargetFrameRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = TraceTypeQuery1; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FVector TraceStartLocal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	TEnumAsByte<EDrawDebugTrace::Type> DebugType = EDrawDebugTrace::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FRuntimeFloatCurve TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FRuntimeFloatCurve TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FRuntimeFloatCurve TraceYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	FRuntimeFloatCurve TracePitch;

	TMap<TWeakObjectPtr<AActor>, TSet<TWeakObjectPtr<UPrimitiveComponent>>> HitActorsAndComponents;

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:

	struct FInstanceProgressInfo
	{
		float CurrentTotalDuration = 0.0f;
		float LastTraceTime = 0.0f;
		TWeakObjectPtr<UAbilitySystemComponent> ASC;
		TMap<TWeakObjectPtr<AActor>, TSet<TWeakObjectPtr<UPrimitiveComponent>>> HitActorsAndComponents;
		FAnimMontageInstance* MontageInstance = nullptr;
	};

	TMap<uint32, FInstanceProgressInfo> ProgressInfoMap;

	void PerformTrace(const USkeletalMeshComponent* MeshComp, float Alpha, FInstanceProgressInfo& ProgressInfo);
};
