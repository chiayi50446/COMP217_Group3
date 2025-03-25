// Copyright Epic Games, Inc. All Rights Reserved.


#include "COMP217_Group3GameModeBase.h"
#include "ShootingTarget.h"
#include "TimerManager.h"


void ACOMP217_Group3GameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds.
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameMode!"));

}
void ACOMP217_Group3GameModeBase::RespawnTarget(FVector SpawnLocation)
{
    GetWorldTimerManager().SetTimerForNextTick([this, SpawnLocation]() {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(
            TimerHandle,
            FTimerDelegate::CreateUObject(this, &ACOMP217_Group3GameModeBase::SpawnTarget, SpawnLocation),
            1.0f,
            false
        );
        });
}

void ACOMP217_Group3GameModeBase::SpawnTarget(FVector SpawnLocation)
{
    GetWorld()->SpawnActor<AShootingTarget>(AShootingTarget::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
}
