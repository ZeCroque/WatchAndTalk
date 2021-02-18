// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatPlayerController.h"

#include "WatGameState.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"
#include "wat/Widgets/QTEWidget.h"

AWatPlayerController::AWatPlayerController()
{
	Widget = nullptr;
	CameraDeskHUDWidget = nullptr;
	ObjectivesHUDWidget = nullptr;
	bIsUsingController = false;
}

void AWatPlayerController::NotifyObjectiveCompleted_Implementation(
    AObjective *Objective)
{
  	Objective->bCompleted = true;

	auto GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	if(GameState->bScientistTurn)
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

	for (auto Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
	     auto* PlayerController = Cast<AWatPlayerController>(*Iterator);
	     if (PlayerController && PlayerController->IsLocalController())
	     {
	     	if(PlayerController->ObjectivesHUDWidget)
	     	{
		    PlayerController->ObjectivesHUDWidget->UpdateObjectiveList();
		}
	     }
	     else
	     {
		  UpdateObjectiveHUD(PlayerController);
	     }
	}
}

void AWatPlayerController::PossessRPC_Implementation(APawn* PawnToPossess, bool bEnableInput)
{
	Possess(PawnToPossess);
         if(bEnableInput)
         {
           PawnToPossess->EnableInput(this);
         }
}

void AWatPlayerController::UpdateObjectiveHUD_Implementation(AWatPlayerController* PlayerController)
{
	if(PlayerController->IsLocalController() && PlayerController->ObjectivesHUDWidget)
	{
		PlayerController->ObjectivesHUDWidget->UpdateObjectiveList();
	}
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
