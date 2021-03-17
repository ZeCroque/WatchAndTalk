// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameInteractable.h"

#include "wat/Core/WatGameState.h"
#include "wat/GameplayActors/Cage.h"

void AEndGameInteractable::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
}

void AEndGameInteractable::OnInteraction()
{
	Super::OnInteraction();

	if(GameState->Cage)
	{
		GameState->Cage->CloseDoors();
	}
}
