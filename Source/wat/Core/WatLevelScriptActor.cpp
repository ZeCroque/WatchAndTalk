// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Core/WatLevelScriptActor.h"
#include "wat/GameplayActors/Interactables/Desk/SecurityCamera.h"
#include "wat/AI/GridManager.h"
#include "wat/AI/MonsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Core/WatGameState.h"
#include "wat/GameplayActors/Triggers/BunkerTriggerBox.h"


class AWatGameState;

void AWatLevelScriptActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld()->IsServer())
	{
		auto* GameState = GetWorld()->GetGameState();
		AWatGameState* MainGameState = Cast<AWatGameState>(GameState);

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASecurityCamera::StaticClass(), FoundActors);
		if (FoundActors.Num() != 0)
		{
			MainGameState->Cameras.Reserve(2);
			MainGameState->Cameras.SetNum(2);
			for (auto& Floor : MainGameState->Cameras)
			{
				Floor.FloorZones.Reserve(4);
				Floor.FloorZones.SetNum(4);
				for(auto& Zone : Floor.FloorZones)
				{
					Zone.ZoneCameras.Reserve(4);
					Zone.ZoneCameras.SetNum(4);
				}
			}

			for (auto* FoundActor : FoundActors)
			{
				auto* FoundCameraActor = Cast<ASecurityCamera>(FoundActor);
				if (FoundCameraActor)
				{
					MainGameState->Cameras[FoundCameraActor->FloorNumber].FloorZones[FoundCameraActor->ZoneNumber].ZoneCameras[FoundCameraActor->CameraNumber] = FoundCameraActor;
				}
			}
		}

		FoundActors.Empty();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjective::StaticClass(), FoundActors);
		if (FoundActors.Num() != 0)
		{
			MainGameState->Objectives.Reserve(FoundActors.Num());
			MainGameState->Objectives.SetNum(FoundActors.Num());
			for (AActor* FoundActor : FoundActors)
			{
				AObjective* FoundObjective = Cast<AObjective>(FoundActor);
				if (FoundObjective)
				{
					MainGameState->Objectives.Add(FoundObjective);
				}
			}
		}

		MainGameState->MonsterCharacter = Cast<AMonsterCharacter>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterCharacter::StaticClass()));
		MainGameState->GridManager = Cast<AGridManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
		MainGameState->BunkerTriggerBox = Cast<ABunkerTriggerBox>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ABunkerTriggerBox::StaticClass()));

		MainGameState->NumberOfActorInBunker = 2;

		MainGameState->bIsInLobby = false;
		MainGameState->bGameStarted = true;
	}
}
