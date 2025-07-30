#include "Character/Enemies/EnemyController.h"
#include "Character/MainCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Character/Enemies/Enemy.h"

AEnemyController::AEnemyController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyController::SetDetection(float Max, float Min, float Angle)
{
	DetectionRadius = Max;
	MaxAngleDegrees = Angle;
	RangeMin = Min;

	
	if (DetectionSphere)
		{
			DetectionSphere->SetSphereRadius(DetectionRadius);
		}
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemy* ControlledCharacter = Cast<AEnemy>(InPawn);
	if (!ControlledCharacter) return;

	if (!DetectionSphere)
	{
		UE_LOG(LogTemp,Warning,TEXT("Sphere Created"));
		DetectionSphere = NewObject<USphereComponent>(ControlledCharacter);
		if (DetectionSphere)
		{
			DetectionSphere->RegisterComponent();
			DetectionSphere->AttachToComponent(ControlledCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			DetectionSphere->SetSphereRadius(DetectionRadius);
			DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			DetectionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
			DetectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			DetectionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
			DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyController::OnOverlapBegin);
			DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyController::OnOverlapEnd);
		}
	}
	if (DebugDrawInterval > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			DebugVisionTimerHandle,
			this,
			&AEnemyController::DrawDebugVision,
			DebugDrawInterval,
			true
		);
	}
}

void AEnemyController::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									  bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (!ControlledCharacter || !OtherActor || OtherActor == ControlledCharacter) return;

	if (CurrentTarget || bHasSeenTarget) return;

	SetCurrentTarget(OtherActor);
}

void AEnemyController::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CurrentTarget && !bHasSeenTarget)
	{
		ResetTarget();
	}
}

void AEnemyController::SetCurrentTarget(AActor* Target)
{
	if (CurrentTarget || bHasSeenTarget) return;

	CurrentTarget = Target;

	if (CurrentTarget)
	{
		StartVisibilityCheck();
	}
}

void AEnemyController::ResetTarget()
{
	StopVisibilityCheck();
	CurrentTarget = nullptr;
	bIsTargetVisible = false;
	bHasSeenTarget = false;
}

void AEnemyController::StartVisibilityCheck()
{
	if (!GetWorld() || !CurrentTarget) return;

	GetWorld()->GetTimerManager().SetTimer(
		VisibilityCheckTimer, this,
		&AEnemyController::PerformVisibilityCheck,
		VisibilityCheckInterval, true);
}

void AEnemyController::StopVisibilityCheck()
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().ClearTimer(VisibilityCheckTimer);
}

void AEnemyController::PerformVisibilityCheck()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn || !CurrentTarget) return;

	const FVector MyLocation = MyPawn->GetActorLocation();
	const FVector TargetLocation = CurrentTarget->GetActorLocation();

	const float DistSquared = FVector::DistSquared(MyLocation, TargetLocation);
	if (DistSquared <= RangeMin * RangeMin)
	{
		bIsTargetVisible = true;
	}
	else
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);
		float Angle = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(MyPawn->GetActorRotation(), LookAtRot).Yaw);
		if (Angle > MaxAngleDegrees)
		{
			bIsTargetVisible = false;
			return;
		}

		const FVector Start = MyLocation + FVector(0, 0, 50);
		const FVector End = TargetLocation + FVector(0, 0, 50);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(MyPawn);
		Params.AddIgnoredActor(this);

		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

#if WITH_EDITOR
		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 0.3f, 0, 3.0f);
#endif

		if (bHit && Hit.GetActor() != CurrentTarget)
		{
			bIsTargetVisible = false;
			return;
		}

		bIsTargetVisible = true;
	}

	if (!bHasSeenTarget && bIsTargetVisible)
	{
		bHasSeenTarget = true;
		OnTargetDetected(CurrentTarget);
		StopVisibilityCheck();
	}
}
void AEnemyController::DrawDebugVision()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	const FVector Start = MyPawn->GetActorLocation() + FVector(0, 0, 50);

	
	if (CurrentTarget)
	{
		const FVector End = CurrentTarget->GetActorLocation() + FVector(0, 0, 50);
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 3.0f);
	}

	const FVector ForwardVector = MyPawn->GetActorForwardVector();
	const FVector RightBoundary = ForwardVector.RotateAngleAxis(MaxAngleDegrees, FVector::UpVector);
	const FVector LeftBoundary = ForwardVector.RotateAngleAxis(-MaxAngleDegrees, FVector::UpVector);

	DrawDebugLine(GetWorld(), Start, Start + ForwardVector * 500, FColor::Blue, false, 0.1f, 0, 3.0f);
	DrawDebugLine(GetWorld(), Start, Start + RightBoundary * 500, FColor::Yellow, false, 0.1f, 0, 3.0f);
	DrawDebugLine(GetWorld(), Start, Start + LeftBoundary * 500, FColor::Yellow, false, 0.1f, 0, 3.0f);

	
	DrawDebugCircle(
		GetWorld(),
		MyPawn->GetActorLocation(),
		RangeMin,
		32,
		FColor::Red,
		false,
		0.1f,
		0,
		2.5f,
		FVector(1,0,0), FVector(0,1,0), 
		false
	);

	
	if (DetectionSphere)
	{
		DrawDebugCircle(
			GetWorld(),
			DetectionSphere->GetComponentLocation(),
			DetectionSphere->GetScaledSphereRadius(),
			32,
			FColor::Cyan,
			false,
			0.1f,
			0,
			2.0f,
			FVector(1,0,0), FVector(0,1,0),
			false
		);
	}
}
