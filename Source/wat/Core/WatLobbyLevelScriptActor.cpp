// Fill out your copyright notice in the Description page of Project Settings.


#include "WatLobbyLevelScriptActor.h"
#include "WatGameState.h"

void AWatLobbyLevelScriptActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(HasAuthority())
	{
		auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
		if(GameState)
		{
			GameState->bIsInLobby = true;
		}
	}
}
