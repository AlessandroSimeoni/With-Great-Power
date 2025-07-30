// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/WeaponSword.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include <Utils/Enum/EAxisRotation.h>

#include "Components/AudioComponent.h"

#pragma region Initialization

AWeaponSword::AWeaponSword()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create static mesh component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	// Setup static mesh physics and collision
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetEnableGravity(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMesh->SetLinearDamping(LinearDamping);
	StaticMesh->SetAngularDamping(AngularDamping);

	// Create sphere collision component
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);

	// Setup sphere physics and collision
	Sphere->SetSimulatePhysics(false);
	Sphere->SetEnableGravity(false);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetLinearDamping(LinearDamping);
	Sphere->SetAngularDamping(AngularDamping);
	Sphere->SetRelativeLocationAndRotation(SphereOffset, FRotator::ZeroRotator);
	Sphere->SetSphereRadius(SphereRadius);

    Sphere->OnComponentHit.AddDynamic(this, &AWeaponSword::SphereBeginCollision);
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponSword::SphereBeginOverlap);

	// Create audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;

	// Initialize state
	CurrentState = ESwordState::Idle;
	bCollisionEnabled = false;
	bPhysicsActive = false;
	StateTimer = 0.0f;
	PhysicsTimer = 0.0f;

	// Initialize movement variables
	ThrowDirection = FVector::ZeroVector;
	ThrowEndLocation = FVector::ZeroVector;
	ThrowVelocity = 0.0f;
	ThrowForce = 0.0f;
	ThrowDamage = 0.0f;
}

void AWeaponSword::BeginPlay()
{
	Super::BeginPlay();
	InitializeWeapon();
}

void AWeaponSword::InitializeWeapon()
{
	if (!IsValid(OwnerActor) && GetWorld())
	{
		PlayerController = GetWorld()->GetFirstPlayerController();

		if (PlayerController)
		{
			SetWeaponOwner(PlayerController->GetPawn());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WeaponSword: Failed to initialize - no PlayerController found"));
			return;
		}
	}

	SetState(ESwordState::Idle);

	if (RotationSound && AudioComponent)
	{
		AudioComponent->SetSound(RotationSound);
	}

	if (IsValid(Sphere))
	{
		Sphere->SetRelativeLocation(SphereOffset);
		Sphere->SetSphereRadius(SphereRadius);
	}

	OnActorBeginOverlap.AddDynamic(this, &AWeaponSword::ActorBeginOverlap);
	OnActorHit.AddDynamic(this, &AWeaponSword::ActorBeginCollision);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Initialized successfully for owner %s at %s"),
	       OwnerActor ? *OwnerActor->GetName() : TEXT("None"),
	       *GetActorLocation().ToString());
}

#pragma endregion Initialization

#pragma region Public Interface

void AWeaponSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StateTimer += DeltaTime;

	switch (CurrentState)
	{
	case ESwordState::Throwing:
		HandleThrowingState(DeltaTime);
		break;
	case ESwordState::Flying:
		HandleFlyingState(DeltaTime);
		break;
	case ESwordState::Sustain:
		HandleSustainState(DeltaTime);
		break;
	case ESwordState::Embedded:
		HandleEmbeddedState(DeltaTime);
		break;
	case ESwordState::Retrieving:
		HandleRetrievingState(DeltaTime);
		break;
	case ESwordState::Idle:
		// Idle state - no processing needed
		break;
	default:
		// Default state - no processing needed
		break;
	}
}

void AWeaponSword::SphereBeginCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("Sphere Hit: %s hit %s"), *GetName(), *OtherActor->GetName());
	BeginCollision(OtherActor);
	CollideActor(OtherActor, Hit);
}

void AWeaponSword::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Sphere Overlap: %s overlapped %s"), *GetName(), *OtherActor->GetName());
	BeginOverlap(OtherActor);
	TriggerEnemy(OtherActor);
}

void AWeaponSword::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
                             bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	UE_LOG(LogTemp, Log, TEXT("Collision: %s on %s - with impulse (%s) and hit (%s)"), *MyComp->GetName(), *Other->GetName(), *NormalImpulse.ToString(), *Hit.ToString())
	BeginCollision(Other);
	CollideActor(Other, Hit);
}

void AWeaponSword::ActorBeginCollision(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("Collision: %s on %s - with impulse (%s) and hit (%s)"), *SelfActor->GetName(), *OtherActor->GetName(), *NormalImpulse.ToString(), *Hit.ToString())
	BeginCollision(OtherActor);
	CollideActor(OtherActor, Hit);
	
}

void AWeaponSword::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("BuiltIn - Notify: %s on %s"), *OwnerActor->GetName(), *OtherActor->GetName())
	BeginOverlap(OtherActor);
	TriggerEnemy(OtherActor);
}

void AWeaponSword::ActorBeginOverlap(AActor* SelfActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("BuiltIn - Notify: %s on %s"), *OwnerActor->GetName(), *OtherActor->GetName())
	BeginOverlap(OtherActor);
	TriggerEnemy(OtherActor);
}

void AWeaponSword::SetWeaponOwner(AActor* NewOwner)
{
	if (!IsValid(NewOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponSword: Attempted to set invalid owner"));
		return;
	}

	OwnerActor = NewOwner;
	UpdateCollisionIgnores(false);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Owner set to %s"), *NewOwner->GetName());
}

void AWeaponSword::ThrowWeapon(const float Velocity, const float Force, const float Damage, const FVector& Direction,
                               const FVector& TargetPoint)
{
	if (!IsReady())
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponSword: Cannot throw - weapon not ready (State: %hhd)"),
		       CurrentState);
		return;
	}

	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponSword: Cannot throw - invalid owner"));
		return;
	}

	if (!IsValidThrow(Direction))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponSword: Invalid throw direction: %s"), *Direction.ToString());
		return;
	}

	// Store throw parameters
	ThrowDirection = Direction.GetSafeNormal();
	ThrowVelocity = Velocity;
	ThrowForce = Force;
	ThrowDamage = Damage;
	ThrowStartLocation = GetActorLocation();
	ThrowEndLocation = CalculateThrowTarget(ThrowDirection, TargetPoint);

	PhysicsTimer = 0.0f;

	HitActors.Empty();

	SetState(ESwordState::Throwing);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Throwing - Velocity: %.1f, Force: %.1f, Damage: %.1f, Target: %s"),
	       ThrowVelocity, ThrowForce, ThrowDamage, *ThrowEndLocation.ToString());
}

void AWeaponSword::RetrieveWeapon()
{
	if (CurrentState == ESwordState::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponSword: Already at owner location"));
		return;
	}

	if (CurrentState == ESwordState::Retrieving)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponSword: Already retrieving"));
		return;
	}

	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponSword: Cannot retrieve - invalid owner"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Starting retrieval from %s"), *GetActorLocation().ToString());
	SetState(ESwordState::Retrieving);
}

void AWeaponSword::ResetWeapon()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Resetting to idle state"));

	DeactivatePhysicsMovement();
	StopAudio();

	ThrowDirection = FVector::ZeroVector;
	ThrowEndLocation = FVector::ZeroVector;
	ThrowVelocity = 0.0f;
	ThrowForce = 0.0f;
	ThrowDamage = 0.0f;

	if (IsValid(OwnerActor))
	{
		SetActorLocation(OwnerActor->GetActorLocation());
		SetActorRotation(OwnerActor->GetActorRotation());
	}

	SetState(ESwordState::Idle);

	OnRetrieveCompleted.Broadcast();
}

void AWeaponSword::CollideActor(AActor* Actor, const FHitResult Hit)
{
	
	if (!IsValid(Actor) || Actor == OwnerActor)
	{
		return;
	}

	if (CurrentState == ESwordState::Idle ||
		CurrentState == ESwordState::Embedded )
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Hit %s at %s"),
		   *Actor->GetName(), *Hit.Location.ToString());

	OnSwordHit.Broadcast(Actor);

	//if (bPhysicsActive)
	{
		const FVector CurrentVelocity = StaticMesh->GetPhysicsLinearVelocity();
		const FVector ReducedVelocity = CurrentVelocity * HitVelocityReduction;
		StaticMesh->SetPhysicsLinearVelocity(ReducedVelocity);

		const FVector CurrentAngularVelocity = StaticMesh->GetPhysicsAngularVelocityInDegrees();
		const FVector ReducedAngularVelocity = CurrentAngularVelocity * HitVelocityReduction;
		StaticMesh->SetPhysicsAngularVelocityInDegrees(ReducedAngularVelocity);
	}

	if (bEmbeddingOnHit)
	{
		SetState(ESwordState::Embedded);
	}

	// TODO: Apply damage here using ThrowDamage or something
}

void AWeaponSword::TriggerEnemy(AActor* Actor)
{
	if (!HitActors.Contains(Actor))
	{
		HitActors.Add(Actor);
	}
}

#pragma endregion Public Interface

#pragma region State Management

void AWeaponSword::SetState(ESwordState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	const ESwordState OldState = CurrentState;
	CurrentState = NewState;
	StateTimer = 0.0f;
	PhysicsTimer = 0.0f;

	ExecuteStateTransition(OldState);

	SetupCollisionForState(NewState);

	OnStateChanged.Broadcast(NewState);

	LogStateChange(OldState, NewState);
}

void AWeaponSword::ExecuteStateTransition(const ESwordState OldState)
{
	switch (OldState)
	{
	case ESwordState::Idle:
		break;

	case ESwordState::Throwing:
		break;

	case ESwordState::Flying:
		break;
		
	case ESwordState::Sustain:
		break;

	case ESwordState::Embedded:
		break;

	case ESwordState::Retrieving:
		break;
		
	default:
		break;
	}

	switch (CurrentState)
	{
	case ESwordState::Idle:
		StopAudio();
		break;

	case ESwordState::Throwing:
		DeactivatePhysicsMovement();
		break;

	case ESwordState::Flying:
		break;
		
	case ESwordState::Sustain:
		break;

	case ESwordState::Embedded:
		StopAudio();
		ActivatePhysicsMovement();
		break;

	case ESwordState::Retrieving:
		DeactivatePhysicsMovement();
		break;
		
	default:
		break;
	}
}

void AWeaponSword::SetupCollisionForState(const ESwordState State)
{
	switch (State)
	{
	case ESwordState::Idle:
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);

		UpdateCollisionIgnores(false);
		break;

	case ESwordState::Throwing:
	case ESwordState::Flying:
	case ESwordState::Sustain:
	case ESwordState::Retrieving:
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		Sphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

		UpdateCollisionIgnores(true);
		break;

	case ESwordState::Embedded:

		StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);

		UpdateCollisionIgnores(true);
		break;
		
	default:
		break;
	}
}

void AWeaponSword::UpdateCollisionIgnores(const bool enabled)
{
	bCollisionEnabled = enabled;
	if (IsValid(OwnerActor))
	{
		StaticMesh->IgnoreActorWhenMoving(OwnerActor, !bCollisionEnabled);
		Sphere->IgnoreActorWhenMoving(OwnerActor, !bCollisionEnabled);
	}
}

#pragma endregion State Management

#pragma region State Handlers

void AWeaponSword::HandleThrowingState(const float DeltaTime)
{
	PhysicsTimer += DeltaTime;

	UpdateKinematicMovement(DeltaTime);
	UpdateRotation(DeltaTime, bCanRotateThrow, ThrowDirection);

	if (PhysicsTimer >= PhysicsActivationDelay)
	{
		SetState(ESwordState::Flying);
		return;
	}

	PlayAudioIfNeeded();
}

void AWeaponSword::HandleFlyingState(const float DeltaTime)
{
	if (bCanSustain)
	{
		SetState(ESwordState::Sustain);
		return;
	}

	UpdateKinematicMovement(DeltaTime);
	UpdateRotation(DeltaTime, bCanRotateThrow, ThrowDirection);

	const float DistanceFromStart = FVector::Dist(ThrowStartLocation, GetActorLocation());
	const float DistancePath = FVector::Dist(ThrowStartLocation, ThrowEndLocation);
	
	if (DistanceFromStart >= DistancePath)
	{
		SetState(ESwordState::Embedded);
		return;
	}

	PlayAudioIfNeeded();
}

void AWeaponSword::HandleSustainState(const float DeltaTime)
{
	if (!bCanSustain)
	{
		SetState(ESwordState::Embedded);
		return;
	}

	UpdateKinematicMovement(DeltaTime);
	UpdateRotation(DeltaTime, bCanRotateThrow, ThrowDirection);

	UpdateMouseTarget();

	if (const float Distance = FVector::Dist(ThrowEndLocation, GetActorLocation()); Distance <= ArriveDistance)
	{
		SetState(ESwordState::Embedded);
		return;
	}

	PlayAudioIfNeeded();
}

void AWeaponSword::HandleEmbeddedState(const float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("Handle Embedding something"))
}

void AWeaponSword::HandleRetrievingState(const float DeltaTime)
{
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponSword: Lost owner during retrieval"));
		ResetWeapon();
		return;
	}

	const FVector CurrentLocation = GetActorLocation();
	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const float Distance = FVector::Dist(CurrentLocation, OwnerLocation);
	if (Distance <= RetrieveDistance)
	{
		ResetWeapon();
		return;
	}

	const FVector Direction = (OwnerLocation - CurrentLocation).GetSafeNormal();

	const float SpeedMultiplier = FMath::Clamp(1.0f + (1000.0f - Distance) / 1000.0f, 1.0f, 3.0f);

	const float CurrentSpeed = RetrieveSpeed * 100.0f * SpeedMultiplier;
	const FVector NewLocation = CurrentLocation + (Direction * CurrentSpeed * DeltaTime);

	SetActorLocation(NewLocation);
	UpdateRotation(DeltaTime, bCanRotateRetrieve, Direction);
}

#pragma endregion State Handlers

#pragma region Movement and Physics

void AWeaponSword::UpdateKinematicMovement(const float DeltaTime)
{
	if (bPhysicsActive)
	{
		return;
	}

	if (ThrowVelocity <= 0.0f || ThrowEndLocation.IsNearlyZero())
	{
		return;
	}

	const FVector CurrentLocation = GetActorLocation();

	FVector MovementDirection = ThrowDirection;
	if (ThrowDirection.IsNearlyZero())
	{
		MovementDirection = (ThrowEndLocation - CurrentLocation).GetSafeNormal();
	}

	const float MovementDistance = ThrowVelocity * DeltaTime;
	const FVector NewLocation = CurrentLocation + (MovementDirection * MovementDistance);

	SetActorLocation(NewLocation);
}

void AWeaponSword::UpdateRotation(const float DeltaTime, const bool CanRotate, const FVector& Direction)
{
	if (!CanRotate)
	{
		if (!Direction.IsNearlyZero())
		{
			const FQuat TargetRotation = Direction.Rotation().Quaternion();
			const FQuat CurrentRotation = GetActorQuat();
			const FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, DeltaTime * 5.0f);
			SetActorRotation(NewRotation);
		}
		return;
	}

	if (RotationAxis == None || RotationSpeed <= 0.0f)
	{
		return;
	}

	const FVector RotationAxisVector = GetRotationAxisVector();
	if (RotationAxisVector.IsNearlyZero())
	{
		return;
	}

	const float RotationAmount = RotationSpeed * DeltaTime;
	const FQuat LookAt = FRotationMatrix::MakeFromX(ThrowDirection).ToQuat();
	const FQuat CurrentRotation = GetActorQuat();
	//const FVector DirectionToTarget = (ThrowEndLocation - GetActorLocation()).GetSafeNormal();

	// Adjust rotation to face the target location
	//const FQuat LookRotation = LookAt * DeltaRotation;
	//const FQuat TargetRotation = DirectionToTarget.Rotation().Quaternion();
	const FQuat TargetRotation = LookAt;
	const FQuat LookRotation = FQuat::Slerp(CurrentRotation, TargetRotation, DeltaTime * 5.0f);
	SetActorRotation(LookRotation);

	// Rotate the Actor in loop
	const FQuat DeltaRotation = FQuat(RotationAxisVector, FMath::DegreesToRadians(RotationAmount));
	const FQuat NewRotation = (CurrentRotation * DeltaRotation).GetNormalized();
	SetActorRotation(NewRotation);
	//StaticMesh->AddLocalRotation(NewRotation);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Look at - (%s)"), *LookAt.ToString())
}

/*
void AWeaponSword::UpdateRotation(const float DeltaTime, const bool CanRotate, const FVector& Direction)
{
	if (!CanRotate)
	{
		if (!Direction.IsNearlyZero())
		{
			const FQuat TargetRotation = Direction.Rotation().Quaternion();
			const FQuat CurrentRotation = GetActorQuat();
			const FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, DeltaTime * 5.0f);
			SetActorRotation(NewRotation);
		}
		return;
	}

	if (RotationAxis == None || RotationSpeed <= 0.0f)
	{
		return;
	}

	const FVector RotationAxisVector = GetRotationAxisVector();
	if (RotationAxisVector.IsNearlyZero())
	{
		return;
	}

	const float RotationAmount = RotationSpeed * DeltaTime;
	const FQuat DeltaRotation = FQuat(RotationAxisVector, FMath::DegreesToRadians(RotationAmount));
	const FQuat LookAt = FRotationMatrix::MakeFromX(ThrowDirection).ToQuat();
	const FQuat CurrentRotation = GetActorQuat();

	// Adjust rotation to face the target location
	const FQuat LookRotation = LookAt * DeltaRotation;
	SetActorRotation(LookRotation);

	// Rotate the Actor in loop
	const FQuat NewRotation = (CurrentRotation * DeltaRotation).GetNormalized();
	SetActorRotation(NewRotation);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Look at - (%s)"), *LookAt.ToString())
}
*/

/*
// NEW
void AWeaponSword::UpdateRotation(const float DeltaTime, const bool CanRotate, const FVector& Direction)
{
	FVector DirectionToTarget = Direction;
	if (!ThrowEndLocation.IsNearlyZero())
	{
		DirectionToTarget = (ThrowEndLocation - GetActorLocation()).GetSafeNormal();
	}
	
	if (!DirectionToTarget.IsNearlyZero())
	{
		const FQuat TargetRotation = DirectionToTarget.Rotation().Quaternion();
		const FQuat CurrentRotation = GetActorQuat();
		const FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, DeltaTime * 5.0f);
		SetActorRotation(NewRotation);
	}

	if (CanRotate && RotationAxis != None && RotationSpeed > 0.0f)
	{
		const FVector RotationAxisVector = GetRotationAxisVector();
		if (!RotationAxisVector.IsNearlyZero())
		{
			const float RotationAmount = RotationSpeed * DeltaTime;
			const FQuat DeltaRotation = FQuat(RotationAxisVector, FMath::DegreesToRadians(RotationAmount));
			const FQuat CurrentRotation = GetActorQuat();
			const FQuat NewRotation = (CurrentRotation * DeltaRotation).GetNormalized();
			SetActorRotation(NewRotation);
		}
	}
}
*/

void AWeaponSword::ActivatePhysicsMovement()
{
	if (bPhysicsActive)
	{
		return;
	}

	bPhysicsActive = true;

	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Block);

	FVector ImpulseDirection = ThrowDirection;
	if (ImpulseDirection.IsNearlyZero())
	{
		ImpulseDirection = (ThrowEndLocation - GetActorLocation()).GetSafeNormal();
	}

	const FVector LinearImpulse = ImpulseDirection * ThrowForce;
	StaticMesh->AddImpulse(LinearImpulse, NAME_None, true);

	const FVector AngularImpulse = FVector::CrossProduct(ImpulseDirection, FVector::UpVector) * (ThrowForce * 0.001f);
	StaticMesh->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Physics activated - Linear: %s, Angular: %s"),
	       *LinearImpulse.ToString(), *AngularImpulse.ToString());
}

void AWeaponSword::DeactivatePhysicsMovement()
{
	if (!bPhysicsActive)
	{
		return;
	}

	bPhysicsActive = false;

	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetEnableGravity(false);
	StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	StaticMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Physics deactivated"));
}

#pragma endregion Movement and Physics

#pragma region Mouse Targeting and Collision

void AWeaponSword::UpdateMouseTarget()
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	float MouseX, MouseY;
	if (!PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	FVector WorldLocation, WorldDirection;
	if (!PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
	{
		return;
	}

	const FVector Start = WorldLocation;
	const FVector End = Start + WorldDirection * 15000.0f;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if (IsValid(OwnerActor))
	{
		CollisionParams.AddIgnoredActor(OwnerActor);
	}
	CollisionParams.bReturnPhysicalMaterial = false;

	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		LastHitResult = Hit;
	}
	
	if (const FVector MouseWorldPosition = GetMouseInWorldPosition(LastHitResult); !MouseWorldPosition.IsNearlyZero())
	{
		ThrowDirection = (ThrowEndLocation - GetActorLocation()).GetSafeNormal();
		ThrowEndLocation = CalculateThrowTarget(ThrowDirection, MouseWorldPosition);
			
		UE_LOG(LogTemp, Log, TEXT("WeaponSword: Update Location with Mouse - Target: %s"), *ThrowEndLocation.ToString());
	}
}

FVector AWeaponSword::GetMouseInWorldPosition(const FHitResult& Hit) const
{
	if (!Hit.bBlockingHit)
	{
		return FVector::ZeroVector;
	}

	const FVector HitLocation = Hit.Location;
	const FVector HitNormal = Hit.Normal.GetSafeNormal();

	return HitLocation + (HitNormal * OffsetTerrain);
}

#pragma endregion Mouse Targeting and Collision

#pragma region Collision and Hits

/*
void AWeaponSword::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                         FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || OtherActor == OwnerActor)
	{
		return;
	}

	if (CurrentState != ESwordState::Throwing &&
		CurrentState != ESwordState::Flying &&
		CurrentState != ESwordState::Sustain)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("WeaponSword: Hit %s at %s"),
	       *OtherActor->GetName(), *Hit.Location.ToString());

	OnSwordHit.Broadcast(OtherActor);

	if (bPhysicsActive)
	{
		const FVector CurrentVelocity = StaticMesh->GetPhysicsLinearVelocity();
		const FVector ReducedVelocity = CurrentVelocity * HitVelocityReduction;
		StaticMesh->SetPhysicsLinearVelocity(ReducedVelocity);

		const FVector CurrentAngularVelocity = StaticMesh->GetPhysicsAngularVelocityInDegrees();
		const FVector ReducedAngularVelocity = CurrentAngularVelocity * HitVelocityReduction;
		StaticMesh->SetPhysicsAngularVelocityInDegrees(ReducedAngularVelocity);
	}

	if (bEmbeddingOnHit)
	{
		SetState(ESwordState::Embedded);
	}

	// TODO: Apply damage here using ThrowDamage
}
*/

#pragma endregion Collision and Hits

#pragma region Utility Functions

FVector AWeaponSword::CalculateThrowTarget(const FVector& Direction, const FVector& TargetPoint) const
{
	if (!TargetPoint.IsNearlyZero())
	{
		return TargetPoint;
	}

	const FVector StartLocation = GetActorLocation();
	const float Distance = FMath::Min(MinThrowDistance, ThrowVelocity * 2.0f);

	return StartLocation + (Direction * Distance);
}

FVector AWeaponSword::GetRotationAxisVector() const
{
	switch (RotationAxis)
	{
	case X:
		return FVector(1.0f, 0.0f, 0.0f);
	case Y:
		return FVector(0.0f, 1.0f, 0.0f);
	case Z:
		return FVector(0.0f, 0.0f, 1.0f);
	default:
		return FVector::ZeroVector;
	}
}

bool AWeaponSword::IsValidThrow(const FVector& Direction)
{
	return !Direction.IsNearlyZero() && Direction.GetSafeNormal().Size() > 0.5f;
}

void AWeaponSword::PlayAudioIfNeeded() const
{
	if (RotationSound && AudioComponent && !AudioComponent->IsPlaying())
	{
		AudioComponent->Play();
	}
}

void AWeaponSword::StopAudio() const
{
	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
}

void AWeaponSword::LogStateChange(const ESwordState OldState, const ESwordState NewState)
{
	UE_LOG(LogTemp, Log, TEXT("WeaponSword: State changed from %hhd to %hhd"), OldState, NewState);
}

#pragma endregion Utility Functions