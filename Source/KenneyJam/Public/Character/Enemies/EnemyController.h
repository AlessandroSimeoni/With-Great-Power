// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class USphereComponent;

UCLASS()
class KENNEYJAM_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyController();
	UPROPERTY(EditAnywhere, Category="Debug")
	float DebugDrawInterval = -1.0f;

	FTimerHandle DebugVisionTimerHandle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Detection")
	float DetectionRadius = 500.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Detection")
	float MaxAngleDegrees = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float RangeMin = 100;


	UPROPERTY()
	TObjectPtr<USphereComponent> DetectionSphere;

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void SetDetection(float Max,float Min,float Angle);
protected:
	virtual void OnPossess(APawn* InPawn) override;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float VisibilityCheckInterval = 0.3f;
	

	UPROPERTY(BlueprintReadOnly, Category = "Detection")
	bool bIsTargetVisible = false;

	UPROPERTY()
	bool bHasSeenTarget = false;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	FTimerHandle VisibilityCheckTimer;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StartVisibilityCheck();
	void StopVisibilityCheck();
	void PerformVisibilityCheck();
	void DrawDebugVision();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
	void OnTargetDetected(AActor* DetectedActor);

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void SetCurrentTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void ResetTarget();

	UFUNCTION(BlueprintPure, Category = "Detection")
	bool IsTargetVisible() const { return bIsTargetVisible; }
};