// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomPosShootingTarget.h"

ARandomPosShootingTarget::ARandomPosShootingTarget()
{
}

ARandomPosShootingTarget::~ARandomPosShootingTarget()
{
}

void ARandomPosShootingTarget::BeginPlay()
{
    AShootingTarget::BeginPlay();
    FVector StartPosition = GetActorLocation();

    RespawnPos();
}

void ARandomPosShootingTarget::RespawnPos()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("RespawnPos"));
    if (RespawnPoints.Num() == 0) return;

    int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
    //FVector StartPosition = GetActorLocation();
    FVector NewSpawnLocation = RespawnPoints[RandomIndex] ;
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("RandomIndex"+ RandomIndex));
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("RandomIndex" + NewSpawnLocation));

    SetActorLocation(NewSpawnLocation);
}
