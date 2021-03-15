// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/LeaveLobbyInteractable.h"

#include "Components/TextRenderComponent.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/WatSettings.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "wat/Core/WatGameState.h"

void ALeaveLobbyInteractable::SetTextRenderToReadyString_Implementation()
{
	TextRender->SetText(FText::FromString(ReadyString));
}

void ALeaveLobbyInteractable::BeginPlay()
{
	Super::BeginPlay();

	TextRender->SetText(FText::FromString(NotReadyString));
}

void ALeaveLobbyInteractable::OnInteraction()
{
	if(HasAuthority())
	{
		if(PlayerCharacter->IsLocallyControlled())
		{
			TextRender->SetText(FText::FromString(ReadyString));
		}
		else
		{
			SetTextRenderToReadyString();
		}
	}
	
	ReadyPlayersIds.Add(PlayerCharacter->GetPlayerState()->GetPlayerId());

	auto& PlayerStates = GetWorld()->GetGameState()->PlayerArray;

	if(PlayerStates.Num() > 1)
	{
		int MatchingPlayersIdsCount = 0;
		for(auto& CurrentPlayerState : PlayerStates)
		{
			for(auto& ReadyPlayerId : ReadyPlayersIds)
			{
				if(ReadyPlayerId == CurrentPlayerState->GetPlayerId())
				{
					++MatchingPlayersIdsCount;
					break;
				}
			}
		}

		if(MatchingPlayersIdsCount == PlayerStates.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("All players are ready, moving to level"));
			const auto* NetworkSettingsWat = GetDefault<UWatSettings>();				
			GetWorld()->ServerTravel(Cast<AWatGameState>(GetWorld()->GetGameState())->bTutorialActivated ? *NetworkSettingsWat->TutoLevel.GetAssetName() : *NetworkSettingsWat->MainLevel.GetAssetName());
		}
	}
}
