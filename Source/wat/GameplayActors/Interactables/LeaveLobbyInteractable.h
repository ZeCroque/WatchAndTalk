// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/GameplayActors/Interactables/Interactable.h"
#include "LeaveLobbyInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ALeaveLobbyInteractable : public AInteractable
{
	GENERATED_BODY()

	TArray<uint32> ReadyPlayersIds;
	
	virtual void OnInteraction() override;
};
