// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainPlayerController.h"

AMainPlayerController::AMainPlayerController()
{
	Widget = nullptr;
}

bool AMainPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
    if (Widget && EventType == EInputEvent::IE_Pressed)
    {
        Widget->OnInput(Key);
        return true;
    }
    return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}
