// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatPlayerController.h"

#include "wat/PauseWidget.h"

AWatPlayerController::AWatPlayerController()
{
	Widget = nullptr;
	PauseWidget = nullptr;
}

void AWatPlayerController::PossessRPC_Implementation(APawn* PawnToPossess)
{
	Possess(PawnToPossess);
}

void AWatPlayerController::BeginPlay()
{
	bIsUsingController = false;
}

bool AWatPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bIsUsingController = bGamepad;
    if (Widget && EventType == EInputEvent::IE_Pressed)
    {
        Widget->OnInput(Key);
        return true;
    }
	if (PauseWidget && EventType == EInputEvent::IE_Pressed)
    {
	    PauseWidget->OnInput(Key);
		return true;
    }
    return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}
