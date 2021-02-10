// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/LeaveLobbyInteractable.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/NetworkSettingsWat.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALeaveLobbyInteractable::OnInteraction()
{
        bInUse = false;
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
			UE_LOG(LogTemp, Warning, TEXT("All players are ready, moving to main level"));
			const auto* NetworkSettingsWat = GetDefault<UNetworkSettingsWat>();				
			GetWorld()->ServerTravel(*NetworkSettingsWat->MainLevel.GetAssetName());
		}
	}
}
