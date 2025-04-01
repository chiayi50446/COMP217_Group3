// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShootingTarget.h"
#include "RandomPosShootingTarget.generated.h"

UCLASS()
class COMP217_GROUP3_API ARandomPosShootingTarget : public AShootingTarget
{
	GENERATED_BODY()

public:
	ARandomPosShootingTarget();
	~ARandomPosShootingTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn", meta = (MakeEditWidget = true))
	TArray<FVector> RespawnPoints;

	void RespawnPos();

protected:
	virtual void BeginPlay() override;
};
