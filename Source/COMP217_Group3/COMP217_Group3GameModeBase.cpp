// Copyright Epic Games, Inc. All Rights Reserved.


#include "COMP217_Group3GameModeBase.h"
#include "ShootingTarget.h"
#include "TimerManager.h"
#include "FPSGameInstance.h"
#include "FPSPlayerState.h"
#include "ShootingTarget.h"
#include <Kismet/GameplayStatics.h>


void ACOMP217_Group3GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FString CurrentLevelName = GetWorld()->GetMapName();
    CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
    if (GameInstance && !CurrentLevelName.Contains("Menu"))
    {
        GameInstance->CurrentLevel = CurrentLevelName;
    }

    if (CurrentLevelName == "Safe_House")
    {
        TimerCount = 30;
    }
    else if (CurrentLevelName == "Demonstration")
    {
        TimerCount = 5;
    }
    else
    {
        return;
    }
    GetWorldTimerManager().SetTimer(CountDownTimerHandle, this,
        &ACOMP217_Group3GameModeBase::CoutDownTimer, 1.0f, true, 1.0f);
}

void ACOMP217_Group3GameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds.
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameMode!"));

}
void ACOMP217_Group3GameModeBase::RespawnTarget(FVector SpawnLocation, TSubclassOf<AShootingTarget> TargetBlueprint)
{
    GetWorldTimerManager().SetTimerForNextTick([this, SpawnLocation, TargetBlueprint]() {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(
            TimerHandle,
            FTimerDelegate::CreateUObject(this, &ACOMP217_Group3GameModeBase::SpawnTarget, SpawnLocation, TargetBlueprint),
            1.0f,
            false
        );
        });
}

void ACOMP217_Group3GameModeBase::SpawnTarget(FVector SpawnLocation, TSubclassOf<AShootingTarget> TargetBlueprint)
{
    if (TargetBlueprint)
    {
        GetWorld()->SpawnActor<AShootingTarget>(TargetBlueprint, SpawnLocation, FRotator::ZeroRotator);
    }
}

void ACOMP217_Group3GameModeBase::GameEnd()
{
    UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        AFPSPlayerState* PlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<AFPSPlayerState>();
        if (PlayerState)
        {
            GameInstance->SavedScore = PlayerState->GetScore();
        }
    }

    UGameplayStatics::OpenLevel(GetWorld(), "EndMenu");
}

void ACOMP217_Group3GameModeBase::CoutDownTimer()
{
    TimerCount--;

    if (TimerCount == 0) {
        GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
        GameEnd();
    }
}
