// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/QTEWidget.h"

AWatPlayerController::AWatPlayerController()
{
	Widget = nullptr;
	CameraDeskHUDWidget = nullptr;
	bIsUsingController = false;
}

void AWatPlayerController::PossessRPC_Implementation(APawn* PawnToPossess)
{
	Possess(PawnToPossess);
}

bool AWatPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bIsUsingController = bGamepad;
    if (Widget && EventType == EInputEvent::IE_Pressed)
    {
        Widget->OnInput(Key);
        return true;
    }
    return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}
