// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainPlayerController.h"

#include "Interactables/Objectives/QTEInteractable.h"

bool AMainPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
    if (bIsInMiniGame && EventType == EInputEvent::IE_Pressed)
    {
        if (!Widget)
        {
            auto* Interactable = Cast<AQTEInteractable>(GetPawn());
            if (Interactable)
            {
                Widget = Interactable->Widget;
            }
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Widget->GetFName().ToString());
        }
        Widget->OnInput(Key);
        return true;
    }
    return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}
