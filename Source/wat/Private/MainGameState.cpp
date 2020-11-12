// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, Player1Cameras);
	DOREPLIFETIME(AMainGameState, Player2Cameras);
}
