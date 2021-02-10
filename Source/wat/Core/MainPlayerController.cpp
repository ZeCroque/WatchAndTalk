// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/MainPlayerController.h"

AMainPlayerController::AMainPlayerController()
{
	Widget = nullptr;
}

void AMainPlayerController::PossessRPC_Implementation(APawn* PawnToPossess)
{
	Possess(PawnToPossess);
}

void AMainPlayerController::BeginPlay()
{
	bIsUsingController = false;
}

bool AMainPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bIsUsingController = bGamepad;
    if (Widget && EventType == EInputEvent::IE_Pressed)
    {
        Widget->OnInput(Key);
        return true;
    }
    return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}
