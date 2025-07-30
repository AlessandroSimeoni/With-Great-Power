#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Engine/EngineTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "EffectData.generated.h"

USTRUCT(BlueprintType)
struct FEffectData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ParticleEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> SoundEffect;

	FEffectData()
	: ParticleEffect(nullptr)
	, SoundEffect(nullptr){}
	
	void SpawnEffect(UObject* WorldContextObject, FVector Location, FRotator Rotation = FRotator::ZeroRotator) const
	{
		if (ParticleEffect)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				WorldContextObject,
				ParticleEffect,
				Location,
				Rotation,
				FVector(1.0f),
				true);

			if (NiagaraComp)
			{
				NiagaraComp->SetAutoDestroy(true);
			}

		}

		if (SoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject, SoundEffect, Location);
		}
	}
	void SpawnEffect(UObject* WorldContextObject, FVector Location,float Radius, FName NiagaraRadiusVariableName, FRotator Rotation = FRotator::ZeroRotator) const
	{
		if (ParticleEffect)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				WorldContextObject,
				ParticleEffect,
				Location,
				Rotation,
				FVector(1.0f),
				true);

			if (NiagaraComp)
			{
				NiagaraComp->SetAutoDestroy(true);
				NiagaraComp->SetFloatParameter(NiagaraRadiusVariableName, Radius);
			}

		}

		if (SoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject, SoundEffect, Location);
		}
	}


};
