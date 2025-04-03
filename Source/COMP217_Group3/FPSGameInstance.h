// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class COMP217_GROUP3_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    int32 SavedScore;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    FString CurrentLevel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString FinalLevel = "Demonstration";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Magazine;
};
