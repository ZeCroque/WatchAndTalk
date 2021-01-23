// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameState.h"

AMainGameState::AMainGameState()
{
	bGameEnded = false;
}

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, Cameras);
	DOREPLIFETIME(AMainGameState, Objectives);
	DOREPLIFETIME(AMainGameState, MonsterCharacter);
}

bool AMainGameState::AreAllObjectivesCompleted() const
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
