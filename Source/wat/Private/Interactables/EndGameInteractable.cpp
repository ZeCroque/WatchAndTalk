// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/EndGameInteractable.h"
#include "Kismet/GameplayStatics.h"

void AEndGameInteractable::Interact()
{
    AInteractable::Interact();
    PlayerController->SetPause(true);
    EndGameWidget = NewObject<UUserWidget>(this, EndGameWidgetTemplate);
    if (!EndGameWidget->GetIsVisible())
    {
        EndGameWidget->AddToViewport();
        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(EndGameWidget->GetCachedWidget());
        PlayerController->SetInputMode(Mode);
        PlayerController->bShowMouseCursor = true;
    }
}