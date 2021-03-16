// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatGameState.h"

#include "WatPlayerController.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"

AWatGameState::AWatGameState()
{
	bGameEnded = false;
	bTutorialActivated = false;
	ObjectiveIndex = 1;
    bGameStarted = false;
}

void AWatGameState::EndGame_Implementation()
{
        auto* EndGameWidget = NewObject<UUserWidget>(this, EndGameWidgetTemplate);
        if (EndGameWidget != nullptr && !EndGameWidget->GetIsVisible())
        {
                EndGameWidget->AddToViewport();
                auto* PC = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
                PC->SetInputMode(FInputModeUIOnly());
                PC->bShowMouseCursor = true;
        }
}

void AWatGameState::UpdateObjective()
{
    Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController())->ObjectivesHUDWidget->UpdateObjectiveList();
}

void AWatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWatGameState, Cameras);
	DOREPLIFETIME(AWatGameState, Objectives);
	DOREPLIFETIME(AWatGameState, MonsterCharacter);
	DOREPLIFETIME(AWatGameState, GridManager);
	DOREPLIFETIME(AWatGameState, bTutorialActivated);
	DOREPLIFETIME(AWatGameState, ObjectiveIndex);
	DOREPLIFETIME(AWatGameState, bScientistTurn);
	DOREPLIFETIME(AWatGameState, bGameStarted);
	DOREPLIFETIME(AWatGameState, bIsInLobby);
	DOREPLIFETIME(AWatGameState, bIsInTutorialMode);
}

bool AWatGameState::AreAllObjectivesCompleted() const
{
	for (AObjective* Interactable : this->Objectives)
	{
		if (Interactable != nullptr && Interactable->bCompleted == false)
		{
			return false;
		}
	}
	return true;
}
