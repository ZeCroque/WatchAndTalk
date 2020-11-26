// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interactables/EndGameInteractable.h"
#include "Net/UnrealNetwork.h"
#include "MainGameState.generated.h"

USTRUCT()
struct FCameraZone
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<class ASecurityCamera*> ZoneCameras;
};

/**
 * 
 */
UCLASS()
class WAT_API AMainGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	TArray<FCameraZone> Player1Cameras;

	UPROPERTY(Replicated)
	TArray<FCameraZone> Player2Cameras;

	UPROPERTY(Replicated)
	TArray<AEndGameInteractable*> EndGameInteractables;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool AreAllEndGameInteractableClicked() const;

};
