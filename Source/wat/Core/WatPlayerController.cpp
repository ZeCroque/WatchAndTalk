// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatPlayerController.h"
#include "wat/PauseWidget.h"
#include "WatGameState.h"
#include "WatPlayerCharacter.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"
#include "wat/Widgets/QTEWidget.h"
#include "Components/CapsuleComponent.h"

AWatPlayerController::AWatPlayerController()
{
	Widget = nullptr;
	PauseWidget = nullptr;
	CameraDeskHUDWidget = nullptr;
	ObjectivesHUDWidget = nullptr;
	bIsUsingController = false;
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

void AWatPlayerController::UpdatePlayerZOffset_Implementation(AWatPlayerCharacter* PlayerCharacter, const float ZOffset)
{
	auto MeshRelativeLocation = PlayerCharacter->GetMesh()->GetRelativeLocation();
	MeshRelativeLocation.Z = ZOffset;
	PlayerCharacter->GetMesh()->SetRelativeLocation(MeshRelativeLocation);
}

void AWatPlayerController::UpdatePlayerCapsuleHalfHeight_Implementation(AWatPlayerCharacter* PlayerCharacter, float Height)
{
	PlayerCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(Height);
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

void AWatPlayerController::UpdateBunkerPlayerCount_Implementation(const int Value)
{
	auto GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	GameState->NumberOfActorInBunker += Value;
}
