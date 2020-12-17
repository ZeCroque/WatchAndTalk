#include "Core/MainPlayerState.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"


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

void AMainPlayerState::CheckIfGameShouldEnd_Implementation(AEndGameInteractable* Interactable)
{
    Interactable->bUsed = true;
    AMainGameState* MainGameState = Cast<AMainGameState>(GetWorld()->GetGameState());
    if (MainGameState->AreAllEndGameInteractableClicked() == true)
    {
        EndGame();
    } 
}

void AMainPlayerState::EndGame_Implementation()
{
    auto* EndGameWidget = NewObject<UUserWidget>(this, EndGameWidgetTemplate);
    if (EndGameWidget != nullptr && !EndGameWidget->GetIsVisible())
    {
        EndGameWidget->AddToViewport();
        auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
    }
}
