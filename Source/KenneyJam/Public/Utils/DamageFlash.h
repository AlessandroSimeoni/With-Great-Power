// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageFlash.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KENNEYJAM_API UDamageFlash : public UActorComponent
{
	GENERATED_BODY()

public:	

	UDamageFlash();
	
	UFUNCTION(BlueprintCallable)
	void SetFlashTargetSkeletalMesh(USkeletalMeshComponent* FleshTarget);

	UFUNCTION(BlueprintCallable)
	void SetFlashTargetStaticMesh(UStaticMeshComponent* FlashTarget);

	
	UFUNCTION(BlueprintCallable)
	void FlashDamage();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UMaterialInstanceDynamic> DynMaterial;

private:
	UPROPERTY(EditDefaultsOnly,Category="Material",meta=(AllowPrivateAccess=true))
	float FlashDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly,Category="Material",meta=(AllowPrivateAccess=true))
	FName ParameterStartTimeName = "HitStartTime";
	UPROPERTY(EditDefaultsOnly,Category="Material",meta=(AllowPrivateAccess=true))
	FName ParameterNameDuration = "HitDuration";



		
};
