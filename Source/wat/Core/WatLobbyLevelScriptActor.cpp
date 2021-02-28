// Fill out your copyright notice in the Description page of Project Settings.


#include "WatLobbyLevelScriptActor.h"
#include "WatGameState.h"

void AWatLobbyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = nullptr;
	if(!HasAuthority())
	{
		PlayerController = GetWorld()->GetFirstPlayerController();
	}
	else
	{
		for (auto Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{

			if ((*Iterator)->IsLocalController())
			{
				PlayerController = (*Iterator).Get();
			}
		}
	}
	
	if (PlayerController)
	{
		PlayerController->EnableInput(nullptr);
		PlayerController->SetInputMode(FInputModeGameOnly());
		auto* Pawn = PlayerController->GetPawn();
		if(Pawn)
		{
			Pawn->SetActorTickEnabled(true);
		}
	}

	auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	if(GameState)
	{
		GameState->bIsInLobby = true;
	}
  
}
