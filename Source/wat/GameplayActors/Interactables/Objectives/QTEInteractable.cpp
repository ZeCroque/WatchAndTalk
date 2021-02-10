// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/Objectives/QTEInteractable.h"
#include "wat/Widgets/QTEWidget.h"
#include "wat/Core/MainPlayerController.h"
#include "wat/Core/MainGameState.h"

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
