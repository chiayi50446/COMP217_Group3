// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COMP217_GROUP3_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    AFPSPlayerState();

    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 PlayerScore;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 ScoreAmount);

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetScore() const;
};
