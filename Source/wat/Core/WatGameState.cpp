// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatGameState.h"

AWatGameState::AWatGameState()
{
	bGameEnded = false;
}

void AWatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWatGameState, Cameras);
	DOREPLIFETIME(AWatGameState, Objectives);
	DOREPLIFETIME(AWatGameState, MonsterCharacter);
	DOREPLIFETIME(AWatGameState, GridManager);
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
