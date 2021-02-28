// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/Objectives/QTEInteractable.h"
#include "wat/Widgets/QTEWidget.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerCharacter.h"

void AQTEInteractable::OnNotFinalBlend()
{
    Super::OnNotFinalBlend();
    MiniGameWidget->Start();
    PlayerController->Widget = MiniGameWidget;
}

void AQTEInteractable::OnFinalBlend()
{
    if(bCompleted)
    {
	    PlayerController->NotifyObjectiveCompleted(this);
	}

	if(!GameState->bGameEnded)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

	Super::OnFinalBlend();
}
