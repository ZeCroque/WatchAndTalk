// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/LeaveLobbyTrigger.h"
#include "Characters/PlayerCharacter.h"
#include "Core/MainGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALeaveLobbyTrigger::Interact()
{
	if (PlayerCharacter)
	{
		PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
		if (PlayerController)
		{
			ReadyPlayersIds.Add(PlayerCharacter->GetPlayerState()->GetPlayerId());

			int MatchingPlayersIdsCount = 0;

			auto& PlayerStates = GetWorld()->GetGameState()->PlayerArray;
			
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
				Cast<UMainGameInstance>(GetGameInstance())->MoveToMainLevel();
			}
		}
	}
}
