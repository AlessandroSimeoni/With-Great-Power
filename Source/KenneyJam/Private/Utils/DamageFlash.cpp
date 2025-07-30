// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/DamageFlash.h"


UDamageFlash::UDamageFlash()
{

	PrimaryComponentTick.bCanEverTick = false;


}

void UDamageFlash::SetFlashTargetSkeletalMesh(USkeletalMeshComponent* FlashTarget)
{
	check(FlashTarget);
	
	DynMaterial = UMaterialInstanceDynamic::Create(FlashTarget->GetMaterial(0), this);
	if (!DynMaterial.IsValid()) return;
	
	FlashTarget->SetMaterial(0, DynMaterial.Get());
}

void UDamageFlash::SetFlashTargetStaticMesh(UStaticMeshComponent* FlashTarget)
{
	check(FlashTarget);
	
	DynMaterial = UMaterialInstanceDynamic::Create(FlashTarget->GetMaterial(0), this);
	if (!DynMaterial.IsValid()) return;
	
	FlashTarget->SetMaterial(0, DynMaterial.Get());
}

void UDamageFlash::FlashDamage()
{
	if (!DynMaterial.IsValid())
	{
		return;
	}
	
	DynMaterial->SetScalarParameterValue(ParameterStartTimeName,GetWorld()->GetTimeSeconds());
	DynMaterial->SetScalarParameterValue(ParameterNameDuration,FlashDuration);
}


