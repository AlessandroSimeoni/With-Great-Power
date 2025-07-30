// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Projectile/StraightTargetProjectile.h"

void AStraightTargetProjectile::Shoot(const UMuzzle* InstigatorMuzzle, const float ProjectileSpeed,
	const AActor* ProjectileTargetActor)
{
	if (!ProjectileTargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("No ProjectileTargetActor for StraightTargetProjectile!"));
		return;
	}

	const FVector ForwardDirection = GetActorForwardVector();
	FVector DesiredDirection = (ProjectileTargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const float AngleDegrees = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ForwardDirection, DesiredDirection)));

	if (AngleDegrees > MaxAngle)
	{
		const FVector Axis = FVector::CrossProduct(ForwardDirection, DesiredDirection).GetSafeNormal();
		const FQuat LimitQuat = FQuat(Axis, FMath::DegreesToRadians(MaxAngle));
		DesiredDirection = LimitQuat.RotateVector(ForwardDirection);
	}

	// rotate projectile towards DesiredDirection
	const FRotator TargetRotation = FRotationMatrix::MakeFromX(DesiredDirection).Rotator();
	SetActorRotation(TargetRotation);
	
	Super::Shoot(InstigatorMuzzle, ProjectileSpeed, ProjectileTargetActor);
}
