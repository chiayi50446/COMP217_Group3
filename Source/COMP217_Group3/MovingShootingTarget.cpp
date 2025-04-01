// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingShootingTarget.h"

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
    FVector TargetLocation = TargetPoints[CurrentTargetIndex];

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
    FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

    SetActorLocation(NewLocation);

    // 設置旋轉，讓物件面向移動方向
    if (!Direction.IsNearlyZero()) // 確保方向有效
    {
        FRotator NewRotation = Direction.Rotation();
        NewRotation.Yaw -= 90.0f;
        SetActorRotation(NewRotation);
    }

    // 檢查是否接近目標點
    if (FVector::Dist(NewLocation, TargetLocation) < 10.f)
    {
        CurrentTargetIndex = (CurrentTargetIndex + 1) % TargetPoints.Num();
    }
}
