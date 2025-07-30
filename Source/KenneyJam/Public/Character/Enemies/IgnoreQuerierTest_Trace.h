// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Tests/EnvQueryTest_Trace.h"
#include "IgnoreQuerierTest_Trace.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Trace (Ignore Querier + Player)"))
class KENNEYJAM_API UIgnoreQuerierTest_Trace : public UEnvQueryTest_Trace
{
	GENERATED_BODY()

public:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
