// Fill out your copyright notice in the Description page of Project Settings.


#include "OctoInteractable.h"


#include "wat/Core/WatGameState.h"
#include "wat/Core/WatSettings.h"
#include "wat/Core/WatStaticUtils.h"

void AOctoInteractable::BeginPlay()
{
	Super::BeginPlay();
	bActivated = !Cast<AWatGameState>(GetWorld()->GetGameState())->bIsInLobby;
}

void AOctoInteractable::OnInteraction()
{
	Super::OnInteraction();

	UWatStaticUtils::ShowLoadingScreenOnAllPlayers(GetWorld());
	const auto* NetworkSettingsWat = GetDefault<UWatSettings>();
	if(Cast<AWatGameState>(GetWorld()->GetGameState())->bIsInLobby)
	{
		GetWorld()->ServerTravel(*NetworkSettingsWat->EasterEggLevel.GetAssetName());
	}
	else
	{
		GetWorld()->ServerTravel(*NetworkSettingsWat->LobbyLevel.GetAssetName());
	}
}

bool AOctoInteractable::CanInteract(AWatPlayerCharacter* Player)
{
	return Super::CanInteract(Player) && bActivated;
}

void AOctoInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOctoInteractable, bActivated);
}
