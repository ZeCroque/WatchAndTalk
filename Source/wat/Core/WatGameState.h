// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "Net/UnrealNetwork.h"
#include "WatGameState.generated.h"


USTRUCT()
struct FZoneCameras
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<class ASecurityCamera*> ZoneCameras;
};

USTRUCT()
struct FFloorZones
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FZoneCameras> FloorZones;
};

/**
 * 
 */
UCLASS()
class WAT_API AWatGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	TArray<FFloorZones> Cameras;

	UPROPERTY(Replicated)
	TArray<AObjective*> Objectives;

	UPROPERTY(Replicated)
	class AMonsterCharacter* MonsterCharacter;

	UPROPERTY(Replicated)
	class AGridManager* GridManager;

	UPROPERTY()
	class ABunkerTriggerBox* BunkerTriggerBox;

	UPROPERTY()
	class ACage* Cage;

	UPROPERTY(Replicated)
	AActor* Gas;
	
	UPROPERTY()
	TSet<uint32> PlayersReadyForRestartIds;

	UPROPERTY()
	int NumberOfActorInBunker;

	UPROPERTY(Replicated)
	bool bTutorialActivated;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bGameStarted;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsInTutorialMode;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsInLobby;
	
	bool bGameEnded;

	UPROPERTY(Replicated)
	uint8 ObjectiveIndex;

	UPROPERTY(ReplicatedUsing = UpdateObjective)
	bool bScientistTurn;

	UPROPERTY(EditAnywhere, Category="WAT Objectives")
	class UDataTable* ObjectiveDataTable;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EndGameWidgetTemplate;

	AWatGameState();

	UFUNCTION(NetMulticast, Reliable)
	void EndGame();

	UFUNCTION()
	void UpdateObjective();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool AreAllObjectivesCompleted() const;
};
