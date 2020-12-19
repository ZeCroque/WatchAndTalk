// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interactables/Objectives/Objective.h"
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
	TArray<FCameraZone> Cameras;

	UPROPERTY(Replicated)
	TArray<AObjective*> Objectives;

	UPROPERTY(Replicated)
	class AMonsterCharacter* MonsterCharacter;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool AreAllObjectivesCompleted() const;

};
