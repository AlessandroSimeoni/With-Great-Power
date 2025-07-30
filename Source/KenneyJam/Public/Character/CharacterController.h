// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

class AMainCharacter;

UCLASS()
class KENNEYJAM_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping|Locomotion")
	TObjectPtr<class UInputMappingContext> LocomotionMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping|Locomotion")
	TObjectPtr<class UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping|Combat")
	TObjectPtr<class UInputMappingContext> CombatMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping|Combat")
	TObjectPtr<UInputAction> ThrowAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping|Combat")
	TObjectPtr<UInputAction> RetrieveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping|Combat")
	TObjectPtr<UInputAction> TeleportAction;
	
	UPROPERTY(BlueprintReadOnly)
	AMainCharacter* PossessedMainCharacter = nullptr;
	FVector MouseWorldLocation;
	FVector MouseWorldDirection;

	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn * PossessedPawn) override;
	virtual void OnUnPossess() override;

	
	class UEnhancedInputLocalPlayerSubsystem * GetInputSubsystem() const;
	
	UFUNCTION()
	void HandleMoveAction(const struct FInputActionValue & Value);
	UFUNCTION()
	void HandleStartThrowingAction(const struct FInputActionValue & Value);
	UFUNCTION()
	void HandleThrowAction(const struct FInputActionValue & Value);
	UFUNCTION()
	void HandleRetrieveAction(const struct FInputActionValue & Value);
	UFUNCTION()
	void HandleTeleportAction(const struct FInputActionValue & Value);

public:
	FVector MovementDirection;
	
	UFUNCTION(BlueprintCallable)
	void ToggleControls(bool Value);
	void SetInputContextEnabled(UInputMappingContext * Context, bool bEnabled, int Priority = 1) const;
};
