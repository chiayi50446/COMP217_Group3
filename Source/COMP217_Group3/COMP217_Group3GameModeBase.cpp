// Copyright Epic Games, Inc. All Rights Reserved.


#include "COMP217_Group3GameModeBase.h"


void ACOMP217_Group3GameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds.
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameMode!"));

}
