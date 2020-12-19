#include "Interactables/Objectives/Objective.h"
#include "Characters/MonsterCharacter.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"

void AObjective::EndGame_Implementation()
{
    auto* EndGameWidget = NewObject<UUserWidget>(this, EndGameWidgetTemplate);
    if (EndGameWidget != nullptr && !EndGameWidget->GetIsVisible())
    {
        EndGameWidget->AddToViewport();
        PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
    }
}

void AObjective::Interact()
{
    Super::Interact();

    bUsed = true;
    auto* MainGameState = Cast<AMainGameState>(GetWorld()->GetGameState());
    if (PlayerController)
    {
        if (MainGameState->AreAllObjectivesCompleted() == true)
        {
            EndGame();
        }
        else
        {
            MainGameState->MonsterCharacter->StartGoalReaction();
        }
    }
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AObjective, bUsed);
}
