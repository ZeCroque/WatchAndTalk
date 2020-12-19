#include "Core/MainPlayerState.h"

void AMainPlayerState::KillPlayer_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Player was kill"));
    //todo kill the player
}

bool AMainPlayerState::KillPlayer_Validate()
{
    return true;
}

void AMainPlayerState::OnDeactivated()
{
    UE_LOG(LogTemp, Warning, TEXT("Client %s disconnected"), *this->GetPlayerName());
}
