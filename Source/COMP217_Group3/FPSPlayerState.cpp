// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerState.h"

AFPSPlayerState::AFPSPlayerState()
{
    PlayerScore = 0;
}

void AFPSPlayerState::AddScore(int32 ScoreAmount)
{
    PlayerScore += ScoreAmount;
}

int32 AFPSPlayerState::GetScore() const
{
    return PlayerScore;
}