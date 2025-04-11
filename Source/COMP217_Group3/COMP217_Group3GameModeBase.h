// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShootingTarget.h"
#include "GameFramework/GameModeBase.h"
#include "COMP217_Group3GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class COMP217_GROUP3_API ACOMP217_Group3GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	virtual void StartPlay() override;
public:
	UFUNCTION()
	void BeginPlay() override;
	void RespawnTarget(FVector SpawnLocation, TSubclassOf<AShootingTarget> TargetBlueprint, TArray<FVector> TargetPoints = TArray<FVector>());
	void AddTimer(int addTime);

	UPROPERTY(BlueprintReadOnly)
	int TimerCount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BGM_Sound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* ClockTickSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* TimesUpSound;

private:
	void SpawnTarget(FVector SpawnLocation, TSubclassOf<AShootingTarget> TargetBlueprint, TArray<FVector> TargetPoints = TArray<FVector>());
	void GameEnd();
	FTimerHandle CountDownTimerHandle = FTimerHandle();
	void CoutDownTimer();
	void PlayClockTick();
	void PlayTimesUpSound();
};
