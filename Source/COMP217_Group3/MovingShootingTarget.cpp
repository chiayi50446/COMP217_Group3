// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingShootingTarget.h"
#include "COMP217_Group3GameModeBase.h"

AMovingShootingTarget::AMovingShootingTarget()
{
}

AMovingShootingTarget::~AMovingShootingTarget()
{
}

void AMovingShootingTarget::BeginPlay()
{
    AShootingTarget::BeginPlay();

    if (TargetPoints.Num() > 0)
    {   
        SetActorLocation(TargetPoints[0]);
        CurrentTargetIndex = 1;

    }
}

void AMovingShootingTarget::Tick(float DeltaTime)
{
    AShootingTarget::Tick(DeltaTime);

    if (TargetPoints.Num() == 0) return;

    FVector CurrentLocation = GetActorLocation();
    UE_LOG(LogTemp, Warning, TEXT("Current Location: %s"), *CurrentLocation.ToString());
    FVector TargetLocation = TargetPoints[CurrentTargetIndex];

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
    FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

    SetActorLocation(NewLocation);

    if (!Direction.IsNearlyZero())
    {
        FRotator NewRotation = Direction.Rotation();
        NewRotation.Yaw -= 90.0f;
        SetActorRotation(NewRotation);
    }

    if (FVector::Dist(NewLocation, TargetLocation) < 10.f)
    {
        CurrentTargetIndex = (CurrentTargetIndex + 1) % TargetPoints.Num();
    }
}

void AMovingShootingTarget::InitTargetPoints(TArray<FVector> Points)
{
    TargetPoints = Points;
}

void AMovingShootingTarget::Respawn()
{
    ACOMP217_Group3GameModeBase* GameMode = GetWorld()->GetAuthGameMode<ACOMP217_Group3GameModeBase>();
    GameMode->RespawnTarget(GetActorLocation(), TargetBlueprint, TargetPoints);
}
