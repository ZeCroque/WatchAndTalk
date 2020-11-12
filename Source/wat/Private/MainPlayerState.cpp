// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerState.h"


void AMainPlayerState::KillPlayer_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Player was kill"));
    //todo kill the player
}

bool AMainPlayerState::KillPlayer_Validate()
{
    return true;
}
