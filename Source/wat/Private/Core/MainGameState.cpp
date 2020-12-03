// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameState.h"

#include "OnlineSubsystem.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, Player1Cameras);
	DOREPLIFETIME(AMainGameState, Player2Cameras);
	DOREPLIFETIME(AMainGameState, EndGameInteractables);
}

bool AMainGameState::AreAllEndGameInteractableClicked() const
{
	for (AEndGameInteractable* Interactable : this->EndGameInteractables)
	{
		if (Interactable != nullptr && Interactable->bUsed == false)
		{
			return false;
		}
	}
	return true;
}