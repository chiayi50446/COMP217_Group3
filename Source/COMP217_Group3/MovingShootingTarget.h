// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShootingTarget.h"
#include "MovingShootingTarget.generated.h"


UCLASS()
class COMP217_GROUP3_API AMovingShootingTarget : public AShootingTarget
{
	GENERATED_BODY()
public:
	AMovingShootingTarget();
	~AMovingShootingTarget();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveSpeed = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (MakeEditWidget = true))
    TArray<FVector> TargetPoints;

protected:
    virtual void BeginPlay() override;

private:
    int CurrentTargetIndex = 0;
    FVector StartLocation;
};
