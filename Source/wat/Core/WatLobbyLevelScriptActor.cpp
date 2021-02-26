// Fill out your copyright notice in the Description page of Project Settings.


#include "WatLobbyLevelScriptActor.h"
#include "WatGameState.h"

void AWatLobbyLevelScriptActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	auto* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->EnableInput(nullptr);
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->GetPawn()->SetActorTickEnabled(true);
	}

	auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	if(GameState)
	{
		GameState->bIsInLobby = true;
	}
  
}
