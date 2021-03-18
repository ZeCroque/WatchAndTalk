// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatPlayerController.h"

#include "WatGameInstance.h"
#include "wat/Widgets/PauseWidget.h"
#include "WatGameState.h"
#include "WatPlayerCharacter.h"
#include "WatSettings.h"
#include "WatStaticUtils.h"
#include "Components/HorizontalBox.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/GameplayActors/Cage.h"
#include "wat/GameplayActors/Interactables/Desk/CameraDesk.h"
#include "wat/Widgets/CameraDeskHUDWidget.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"
#include "wat/Widgets/Tutorial/TutoMessageWidget.h"
#include "wat/Widgets/MiniGames/QTEWidget.h"

AWatPlayerController::AWatPlayerController()
{
	MiniGameWidget = nullptr;
	PauseWidget = nullptr;
	CameraDeskHUDWidget = nullptr;
	ObjectivesHUDWidget = nullptr;
	MainHUDWidget = nullptr;
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

	if(GameState->bGameStarted)
	{
		auto MaxIndex = 0;
		for(auto* LocalObjective : GameState->Objectives)
		{
			if(LocalObjective && LocalObjective->ObjectiveIndex >= MaxIndex)
			{
				MaxIndex = LocalObjective->ObjectiveIndex;
			}
		}
		if(GameState->ObjectiveIndex == MaxIndex)
		{
			auto* CameraDesk = Cast<ACameraDesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraDesk::StaticClass()));
			CameraDesk->bShouldShowEndGameInteraction = true;

			for (auto Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				auto* PlayerController = Cast<AWatPlayerController>(*Iterator);
				if (PlayerController)
				{
					PlayerController->ShowCameraDeskHUDEndGameInteraction();
				}
			}

		}
	}

	if(GameState->bIsInTutorialMode && !GameState->bGameStarted)
	{
		if(GameState->ObjectiveIndex == 1)
		{

			UWatStaticUtils::ShowLoadingScreenOnAllPlayers(GetWorld());
			const auto* NetworkSettingsWat = GetDefault<UWatSettings>();
			GetWorld()->ServerTravel(*NetworkSettingsWat->MainLevel.GetAssetName());
			return;
		}
		Objective->bIsOnlyForMechanist = false;
		Objective->bCompleted = false;
	}

	GameState->bScientistTurn = !GameState->bScientistTurn;
	
	for(auto* ObjectiveInteractable : GameState->Objectives)
	{
		if(ObjectiveInteractable)
		{
			ObjectiveInteractable->CheckInteractionState();
		}
	}
	
	if (GameState->bGameStarted && GameState->AreAllObjectivesCompleted() == true)
	{
		GameState->bGameEnded = true;
		GameState->EndGame();
		return;
	}

	Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController())->ObjectivesHUDWidget->UpdateObjectiveList();
}

void AWatPlayerController::ShowCameraDeskHUDEndGameInteraction_Implementation()
{
	if(IsLocalController() && CameraDeskHUDWidget)
	{
		CameraDeskHUDWidget->EndGameHBox->SetVisibility(ESlateVisibility::Visible);
		if(CameraDeskHUDWidget->IsInViewport())
		{
			CameraDeskHUDWidget->RemoveFromParent();
			CameraDeskHUDWidget->AddToViewport();
		}
	}
}

void AWatPlayerController::PossessRPC_Implementation(APawn* PawnToPossess, bool bEnableInput)
{
	Possess(PawnToPossess);
	if (bEnableInput)
	{
		PawnToPossess->EnableInput(this);
	}
}

void AWatPlayerController::ShowLoadingWidget_Implementation()
{
	if(IsLocalController())
	{
		if(MainHUDWidget)
		{
			MainHUDWidget->RemoveFromParent();
			MainHUDWidget = nullptr;
		}

		if(TutoMessageWidget)
		{
			TutoMessageWidget->RemoveFromParent();
			TutoMessageWidget = nullptr;
		}

		if(MiniGameWidget)
		{
			MiniGameWidget->RemoveFromParent();
			MiniGameWidget = nullptr;
		}

		if(CameraDeskHUDWidget)
		{
			CameraDeskHUDWidget->RemoveFromParent();
			CameraDeskHUDWidget = nullptr;
		}

		if(PauseWidget)
		{
			PauseWidget->RemoveFromParent();
			PauseWidget = nullptr;
		}

		if(ObjectivesHUDWidget)
		{
			ObjectivesHUDWidget->RemoveFromParent();
			ObjectivesHUDWidget = nullptr;
		}

		if(EndGameWidget)
		{
			EndGameWidget->RemoveFromParent();
			EndGameWidget = nullptr;
		}

		if(GameOverWidget)
		{
			GameOverWidget->RemoveFromParent();
			GameOverWidget = nullptr;
		}
		
		auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
		if(GameInstance)
		{
			GameInstance->LoadingScreenWidget = CreateWidget<UUserWidget>(this, LoadingScreenClass);
			GameInstance->LoadingScreenWidget->AddToViewport();
		}
	}
}

void AWatPlayerController::CloseCage_Implementation()
{
	auto GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	GameState->Cage->CloseDoors();
}

void AWatPlayerController::SetPlayerAsReadyForRestart()
{
	RestartGame(GetUniqueID());
}

void AWatPlayerController::RestartGame_Implementation(const uint32 PlayerId)
{
	auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	GameState->PlayersReadyForRestartIds.Add(PlayerId);


	if (GameState->PlayersReadyForRestartIds.Num() ==GetWorld()->GetNumPlayerControllers())
	{
		UE_LOG(LogTemp, Warning, TEXT("All players are ready to restart, moving to lobby level"));
		auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
		GameInstance->bIsInTutorialMode = false;
		GameInstance->bHostIsScientist = false;
		
		const auto* NetworkSettingsWat = GetDefault<UWatSettings>();
		UWatStaticUtils::ShowLoadingScreenOnAllPlayers(GetWorld());
		GetWorld()->ServerTravel(*NetworkSettingsWat->LobbyLevel.GetAssetName());
		GameState->PlayersReadyForRestartIds.Empty();
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

	if(TutoMessageWidget && EventType == EInputEvent::IE_Pressed)
	{
		TutoMessageWidget->OnInput(Key);
	}
	return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void AWatPlayerController::UpdateBunkerPlayerCount_Implementation(const int Value)
{
	auto GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	GameState->NumberOfActorInBunker += Value;
}
