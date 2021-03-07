// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/PauseWidget.h"
#include "WatGameState.h"
#include "WatPlayerCharacter.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"
#include "wat/Widgets/MiniGames/QTEWidget.h"

AWatPlayerController::AWatPlayerController()
{
	MiniGameWidget = nullptr;
	PauseWidget = nullptr;
	CameraDeskHUDWidget = nullptr;
	ObjectivesHUDWidget = nullptr;
	bIsUsingController = false;
}

void AWatPlayerController::NotifyPossessionEnd_Implementation(ABlendableInteractable* Interactable)
{
	Interactable->SetUseState(false);
}

void AWatPlayerController::BroadcastInteractionEnd_Implementation(AWatPlayerCharacter* InteractingPlayer, ABlendableInteractable* Interactable)
{
	InteractingPlayer->OnInteractionEndDispatcher.Broadcast(Interactable);
}

void AWatPlayerController::NotifyObjectiveCompleted_Implementation(
	AObjective* Objective)
{
	Objective->bCompleted = true;

	auto GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	if (GameState->bScientistTurn)
	{
		GameState->ObjectiveIndex++;
	}
	GameState->bScientistTurn = !GameState->bScientistTurn;

	for(auto* ObjectiveInteractable : GameState->Objectives)
	{
		if(ObjectiveInteractable)
		{
			ObjectiveInteractable->CheckInteractionState();
		}
	}
	
	if (GameState->AreAllObjectivesCompleted() == true)
	{
		GameState->bGameEnded = true;

		GameState->EndGame();
		return;
	}

	Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController())->ObjectivesHUDWidget->UpdateObjectiveList();
}

void AWatPlayerController::PossessRPC_Implementation(APawn* PawnToPossess, bool bEnableInput)
{
	Possess(PawnToPossess);
	if (bEnableInput)
	{
		PawnToPossess->EnableInput(this);
	}
}

bool AWatPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bIsUsingController = bGamepad;
	if (MiniGameWidget && EventType == EInputEvent::IE_Pressed)
	{
		MiniGameWidget->OnInput(Key);
		return true;
	}
	return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void AWatPlayerController::UpdateBunkerPlayerCount_Implementation(const int Value)
{
	auto GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	GameState->NumberOfActorInBunker += Value;
}
