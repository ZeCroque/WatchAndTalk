// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainPlayerState.h"


void AMainPlayerState::KillPlayer_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Player was kill"));
    //todo kill the player
}

bool AMainPlayerState::KillPlayer_Validate()
{
    return true;
}

void AMainPlayerState::Possess_Implementation(APlayerController* ClientController, APawn* Pawn)
{
    Pawn->SetOwner(ClientController);
    ClientController->Possess(Pawn);
}

bool AMainPlayerState::Possess_Validate(APlayerController* ClientController, APawn* Pawn)
{
    return true;
}

void AMainPlayerState::OnDeactivated()
{
	UE_LOG(LogTemp, Warning, TEXT("Client %s deconnected"), *this->GetPlayerName());
}
