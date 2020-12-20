#include "Core/MainPlayerState.h"

void AMainPlayerState::OnDeactivated()
{
    UE_LOG(LogTemp, Warning, TEXT("Client %s disconnected"), *this->GetPlayerName());
}
