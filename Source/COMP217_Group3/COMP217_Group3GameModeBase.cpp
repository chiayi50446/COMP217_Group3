// Copyright Epic Games, Inc. All Rights Reserved.


#include "COMP217_Group3GameModeBase.h"
#include "ShootingTarget.h"
#include "MovingShootingTarget.h"
#include "RandomPosShootingTarget.h"
#include "TimerManager.h"
#include "FPSGameInstance.h"
#include "FPSPlayerState.h"
#include "ShooterPlayerController.h"
#include <Kismet/GameplayStatics.h>


void ACOMP217_Group3GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FString CurrentLevelName = GetWorld()->GetMapName();
    CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
    AShooterPlayerController* playerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (GameInstance && !CurrentLevelName.Contains("Menu"))
    {
        GameInstance->CurrentLevel = CurrentLevelName;
        if (playerController) {
            playerController->SetModeGame();
        }
    }
    else {
        if (playerController) {
            playerController->SetModeUI();
        }
    }

    if (CurrentLevelName == "Safe_House")
    {
        TimerCount = 30;
        if (GameInstance) GameInstance->Magazine = 5;
    }
    else if (CurrentLevelName == "Demonstration")
    {
        TimerCount = 90;
        if (GameInstance) GameInstance->Magazine = 10;
    }
    else
    {
        return;
    }
    GetWorldTimerManager().SetTimer(CountDownTimerHandle, this,
        &ACOMP217_Group3GameModeBase::CoutDownTimer, 1.0f, true, 1.0f);

    if (BGM_Sound)
    {
        UGameplayStatics::PlaySound2D(this, BGM_Sound, 0.7f, 1.0f, 0.0f);
    }
}

void ACOMP217_Group3GameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds.
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameMode!"));

}
void ACOMP217_Group3GameModeBase::RespawnTarget(FVector SpawnLocation, TSubclassOf<AShootingTarget> TargetBlueprint, TArray<FVector> TargetPoints)
{
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateUObject(this, &ACOMP217_Group3GameModeBase::SpawnTarget, SpawnLocation, TargetBlueprint, TargetPoints),
        5.0f,
        false
    );
}

void ACOMP217_Group3GameModeBase::SpawnTarget(FVector SpawnLocation, TSubclassOf<AShootingTarget> TargetBlueprint, TArray<FVector> TargetPoints)
{
    if (!TargetBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnTarget Failed: TargetBlueprint is null!"));
        return;
    }

    if (UWorld* World = GetWorld())
    {
        AShootingTarget* NewTarget = World->SpawnActor<AShootingTarget>(TargetBlueprint, SpawnLocation, FRotator::ZeroRotator);
        if (!NewTarget)
        {
            UE_LOG(LogTemp, Error, TEXT("SpawnTarget Failed: NewTarget spawn failed!"));
            return;
        }
        if (AMovingShootingTarget* MovingTarget = Cast<AMovingShootingTarget>(NewTarget))
        {
            MovingTarget->InitTargetPoints(TargetPoints);
        }
        if (ARandomPosShootingTarget* RandomPosTarget = Cast<ARandomPosShootingTarget>(NewTarget))
        {
            RandomPosTarget->InitRespawnPoints(TargetPoints);
        }
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

    if (TimerCount == 1) {
        PlayTimesUpSound();
    }
    else if (TimerCount <= 10) {
        PlayClockTick();
    }
}

void ACOMP217_Group3GameModeBase::AddTimer(int addTime)
{
    TimerCount += addTime;
}

void ACOMP217_Group3GameModeBase::PlayClockTick()
{
    UGameplayStatics::PlaySound2D(this, ClockTickSound);
}

void ACOMP217_Group3GameModeBase::PlayTimesUpSound()
{
    UGameplayStatics::PlaySound2D(this, TimesUpSound);
}
