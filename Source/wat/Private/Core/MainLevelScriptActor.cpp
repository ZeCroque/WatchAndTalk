// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainLevelScriptActor.h"
#include "SecurityCamera.h"
#include "AI/GridManager.h"
#include "Characters/MonsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Core/MainGameState.h"


class AMainGameState;

void AMainLevelScriptActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (GetWorld()->IsServer())
    {
        auto* GameState = GetWorld()->GetGameState();
        AMainGameState* MainGameState = Cast<AMainGameState>(GameState);

        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASecurityCamera::StaticClass(), FoundActors);
        if (FoundActors.Num() != 0)
        {
        	MainGameState->Cameras.Reserve(2);
        	MainGameState->Cameras.SetNum(2);
        	for(auto& Floor : MainGameState->Cameras)
        	{
        		Floor.FloorZones.Reserve(4);
        		Floor.FloorZones.SetNum(4);
        	}
        	
            for (auto* FoundActor : FoundActors)
            {
                auto* FoundCameraActor = Cast<ASecurityCamera>(FoundActor);
                if (FoundCameraActor)
                {
                    MainGameState->Cameras[FoundCameraActor->FloorNumber].FloorZones[FoundCameraActor->ZoneNumber].ZoneCameras.Add(FoundCameraActor);
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
    }

    auto* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->EnableInput(nullptr);
        PlayerController->SetInputMode(FInputModeGameOnly());
    }
}
