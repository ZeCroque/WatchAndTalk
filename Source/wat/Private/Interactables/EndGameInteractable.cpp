#include "Interactables/EndGameInteractable.h"
#include "Core/MainGameState.h"
#include "Core/MainPlayerState.h"
#include "Kismet/GameplayStatics.h"

void AEndGameInteractable::Interact()
{
    AInteractable::Interact();
    if (PlayerController)
    {
        PlayerController->GetPlayerState<AMainPlayerState>()->CheckIfGameShouldEnd(this);
    }
}

void AEndGameInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEndGameInteractable, bUsed);
}
