// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Objectives/QTEInteractable.h"
#include "QTEWidget.h"
#include "Core/MainPlayerController.h"
#include "Core/MainGameState.h"

void AQTEInteractable::OnNotFinalBlend()
{
	Super::OnNotFinalBlend();
	MiniGameWidget->Start();
    PlayerController->Widget = MiniGameWidget;
}

void AQTEInteractable::OnFinalBlend()
{
	Super::OnFinalBlend();

	if(!GameState->bGameEnded)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}
