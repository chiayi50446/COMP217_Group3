#include "ShooterPlayerController.h"

void AShooterPlayerController::SetModeGame()
{
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
}

void AShooterPlayerController::SetModeUI()
{
    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = true;
}