// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <Components/PrimitiveComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Containers/EnumAsByte.h>
#include <Engine/HitResult.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>

#include "GameFramework/Actor.h"
#include "Utils/Enum/EAxisRotation.h"
#include "Sound/SoundCue.h"
#include "WeaponSword.generated.h"

class USphereComponent;
class USoundCue;
class UAudioComponent;

UENUM(BlueprintType)
enum class ESwordState : uint8
{
	Idle UMETA(DisplayName = "Idle", ToolTip = "Weapon is ready to be thrown"),
	Throwing UMETA(DisplayName = "Throwing", ToolTip = "Initial throw phase before physics activation"),
	Flying UMETA(DisplayName = "Flying", ToolTip = "Weapon is in flight towards target"),
	Sustain UMETA(DisplayName = "Sustain", ToolTip = "Weapon can continue rotating following the cursor"),
	Embedded UMETA(DisplayName = "Embedded", ToolTip = "Weapon has hit something and is embedded"),
	Retrieving UMETA(DisplayName = "Retrieving", ToolTip = "Weapon is returning to owner")
};

UCLASS()
class KENNEYJAM_API AWeaponSword : public AActor
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRetrieveCompleted);

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events",
		meta = (ToolTip = "Called when weapon successfully returns to owner"))
	FOnRetrieveCompleted OnRetrieveCompleted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwordHit, AActor*, HitActor);

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events",
		meta = (ToolTip = "Called when weapon hits an actor during flight"))
	FOnSwordHit OnSwordHit;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, ESwordState, NewState);

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events",
		meta = (ToolTip = "Called whenever weapon state changes"))
	FOnStateChanged OnStateChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
		meta = (ToolTip = "Main visual mesh of the weapon"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
		meta = (ToolTip = "Collision sphere for hit detection"))
	USphereComponent* Sphere;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Setup",
		meta = (AllowPrivateAccess, ToolTip = "Actor that owns this weapon (auto-assigned if empty)"))
	AActor* OwnerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Setup",
		meta = (AllowPrivateAccess, ToolTip = "Player Controller of the current Owning Actor (auto-assigned if empty)"))
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Setup",
		meta = (AllowPrivateAccess, ToolTip = "Sound played during weapon rotation"))
	USoundCue* RotationSound; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Setup",
		meta = (AllowPrivateAccess, ToolTip = "Audio component for playing sounds"))
	UAudioComponent* AudioComponent;

	// Core movement parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Movement",
		meta = (AllowPrivateAccess, ToolTip = "Enable sustain to reach the mouse"))
	bool bCanSustain = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Movement",
		meta = (AllowPrivateAccess, ClampMin = "0.0", ClampMax = "1000.0", Units = "cm",
			ToolTip = "Offset of the actor while the sword is Sustaining"))
	float OffsetTerrain = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Movement",
		meta = (AllowPrivateAccess, ClampMin = "100.0", ClampMax = "100000.0", Units = "cm",
			ToolTip = "Default distance weapon travels if no target specified"))
	float MinThrowDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Movement",
		meta = (AllowPrivateAccess, ClampMin = "10.0", ClampMax = "1000.0", Units = "cm",
			ToolTip = "Distance to target when weapon switches to embedded state"))
	float ArriveDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Movement",
		meta = (AllowPrivateAccess, ClampMin = "0.01", ClampMax = "100.0", Units = "m/s",
			ToolTip = "Speed when weapon returns to owner"))
	float RetrieveSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Movement",
		meta = (AllowPrivateAccess, ClampMin = "10.0", ClampMax = "1000.0", Units = "cm",
			ToolTip = "Distance to owner when weapon completes retrieval"))
	float RetrieveDistance = 100.0f;

	// Rotation settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Rotation",
		meta = (AllowPrivateAccess, ToolTip = "Axis around which the weapon rotates"))
	TEnumAsByte<EAxisRotation> RotationAxis = Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Rotation",
		meta = (AllowPrivateAccess, ToolTip = "Enable spinning rotation during throw/flight"))
	bool bCanRotateThrow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Rotation",
		meta = (AllowPrivateAccess, ToolTip = "Enable rotation during retrieval phase"))
	bool bCanRotateRetrieve = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Rotation",
		meta = (AllowPrivateAccess, ClampMin = "0.0", ClampMax = "3600.0", Units = "deg/s",
			ToolTip = "Degrees per second rotation speed"))
	float RotationSpeed = 720.0f;

	// Physics settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Physics",
		meta = (AllowPrivateAccess, ClampMin = "0.0", ClampMax = "60.0", Units = "s",
			ToolTip = "Time before physics simulation activates during throw"))
	float PhysicsActivationDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Physics",
		meta = (AllowPrivateAccess, ClampMin = "0.0", ClampMax = "2.0",
			ToolTip = "Linear damping applied when physics is active (0=no damping, 1=high damping)"))
	float LinearDamping = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Physics",
		meta = (AllowPrivateAccess, ClampMin = "0.0", ClampMax = "2.0",
			ToolTip = "Angular damping applied when physics is active (0=no damping, 1=high damping)"))
	float AngularDamping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Physics",
		meta = (AllowPrivateAccess, ClampMin = "0.0", ClampMax = "2.0",
			ToolTip = "Velocity reduction multiplier when weapon hits something (0=stops, 1=no reduction)"))
	float HitVelocityReduction = 0.3f;

	// Collision settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Collision",
		meta = (AllowPrivateAccess, ToolTip = "Should weapon become embedded when hitting objects"))
	bool bEmbeddingOnHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Collision",
		meta = (AllowPrivateAccess, ClampMin = "10.0", ClampMax = "1000.0", Units = "cm",
			ToolTip = "Radius of collision sphere for hit detection"))
	float SphereRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Collision",
		meta = (AllowPrivateAccess, ToolTip = "Offset of collision sphere relative to weapon mesh"))
	FVector SphereOffset = FVector(0.0f, 0.0f, 120.0f);

	// Runtime variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State",
		meta = (AllowPrivateAccess, ToolTip = "Current state of the weapon"))
	ESwordState CurrentState = ESwordState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State",
		meta = (AllowPrivateAccess, ToolTip = "Time spent in current state"))
	float StateTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State",
		meta = (AllowPrivateAccess, ToolTip = "Whether physics simulation is currently active"))
	bool bPhysicsActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State",
		meta = (AllowPrivateAccess, ToolTip = "Whether collision detection is currently enabled"))
	bool bCollisionEnabled = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime Start Location of the throwned weapon"))
	FVector ThrowStartLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime End Location of the throwned weapon"))
	FVector ThrowEndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime Direction of the throwned weapon"))
	FVector ThrowDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime velocity of the throwned weapon"))
	float ThrowVelocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime force of the throwned weapon"))
	float ThrowForce;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime damage of the throwned weapon"))
	float ThrowDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Runtime physics timer of the throwned weapon"))
	float PhysicsTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Last hit resulted from the mouse position click"))
	FHitResult LastHitResult;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Runtime",
		meta = (AllowPrivateAccess, ToolTip = "Collection of the actors hitted on a single throw"))
	TArray<AActor*> HitActors;

public:
	AWeaponSword();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Weapon|Core",
		meta = (ToolTip = "Sphere collider Hit event"))
	void SphereBeginCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(Category = "Weapon|Core",
		meta = (ToolTip = "Sphere collider Hit event"))
	void SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Category = "Weapon|Core",
		meta = (ToolTip = "Built-in Hit event"))
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION(Category = "Weapon|Core",
		meta = (ToolTip = "Hit event"))
	void ActorBeginCollision(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Core",
		meta = (ToolTip = "Overlapping event"))
	void BeginCollision(AActor* OtherActor);

	UFUNCTION(Category = "Weapon|Core",
		meta = (ToolTip = "Built-in Hit event"))
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UFUNCTION(Category = "Weapon|Core",
		meta = (ToolTip = "Overlapping event"))
	void ActorBeginOverlap(AActor* SelfActor, AActor* OtherActor);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Core",
		meta = (ToolTip = "Overlapping event"))
	void BeginOverlap(AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Core",
		meta = (ToolTip = "Set the actor that owns this weapon"))
	void SetWeaponOwner(AActor* NewOwner);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Core",
		meta = (ToolTip = "Throw the weapon in specified direction with given parameters"))
	void ThrowWeapon(const float Velocity, const float Force, const float Damage, const FVector& Direction,
	                 const FVector& TargetPoint = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Core",
		meta = (ToolTip = "Call the weapon back to its owner"))
	void RetrieveWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Core",
		meta = (ToolTip = "Instantly reset weapon to owner's location"))
	void ResetWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Destructible",
		meta = (ToolTip = "On hit collision"))
	void CollideActor(AActor* Actor, FHitResult Hit);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Destructible",
		meta = (ToolTip = "On hit trigger"))
	void TriggerEnemy(AActor* Actor);
	
	// State queries
	UFUNCTION(BlueprintPure, Category = "Weapon|State",
		meta = (ToolTip = "Get current weapon state"))
	ESwordState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Weapon|State",
		meta = (ToolTip = "Check if weapon is ready to be thrown"))
	bool IsReady() const { return CurrentState == ESwordState::Idle; }

	UFUNCTION(BlueprintPure, Category = "Weapon|State",
		meta = (ToolTip = "Check if weapon is currently in attack mode (throwing/flying)"))
	bool IsAttacking() const { return CurrentState == ESwordState::Throwing || CurrentState == ESwordState::Flying; }

	UFUNCTION(BlueprintPure, Category = "Weapon|State",
		meta = (ToolTip = "Check if weapon is embedded in something"))
	bool IsEmbedded() const { return CurrentState == ESwordState::Embedded; }

	UFUNCTION(BlueprintPure, Category = "Weapon|State",
		meta = (ToolTip = "Check if weapon is returning to owner"))
	bool IsRetrieving() const { return CurrentState == ESwordState::Retrieving; }

protected:
	UFUNCTION(BlueprintCallable, Category = "Weapon|Mouse")
	void UpdateMouseTarget();

	UFUNCTION(BlueprintPure, Category = "Weapon|Mouse")
	FVector GetMouseInWorldPosition(const FHitResult& Hit) const;

private:
	void InitializeWeapon();
	void SetState(const ESwordState NewState);
	void ExecuteStateTransition(const ESwordState OldState);

	// State handlers
	void HandleThrowingState(const float DeltaTime);
	void HandleFlyingState(const float DeltaTime);
	void HandleSustainState(const float DeltaTime);
	void HandleEmbeddedState(const float DeltaTime);
	void HandleRetrievingState(const float DeltaTime);

	// Collision setup
	void SetupCollisionForState(const ESwordState State);
	void UpdateCollisionIgnores(const bool enabled);

	// Movement functions
	void UpdateKinematicMovement(const float DeltaTime);
	void UpdateRotation(const float DeltaTime, const bool CanRotate, const FVector& Direction);
	void ActivatePhysicsMovement();
	void DeactivatePhysicsMovement();

	// Utility functions
	FVector CalculateThrowTarget(const FVector& Direction, const FVector& TargetPoint) const;
	FVector GetRotationAxisVector() const;
	static bool IsValidThrow(const FVector& Direction);
	void PlayAudioIfNeeded() const;
	void StopAudio() const;

	// Debug
	static void LogStateChange(const ESwordState OldState, const ESwordState NewState);
};
