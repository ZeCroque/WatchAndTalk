// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainLevelScriptActor.h"
#include "SecurityCamera.h"
#include "Characters/MonsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Core/MainGameState.h"


class AMainGameState;
void AMainLevelScriptActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if(GetWorld()->IsServer())
    {
        auto* GameState = GetWorld()->GetGameState();
        AMainGameState* MainGameState = Cast<AMainGameState>(GameState);

        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASecurityCamera::StaticClass(), FoundActors);
        if (FoundActors.Num() != 0)
        {
            MainGameState->Cameras.Reserve(FoundActors.Num());
            MainGameState->Cameras.SetNum(FoundActors.Num());
            for (auto* FoundActor : FoundActors)
            {
                auto* FoundCameraActor = Cast<ASecurityCamera>(FoundActor);
                if (FoundCameraActor)
                {
                    MainGameState->Cameras[FoundCameraActor->ZoneNumber].ZoneCameras.Add(FoundCameraActor);
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
    	
        FoundActors.Empty();
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterCharacter::StaticClass(), FoundActors);
        if (FoundActors.Num() == 1)
        {
            AMonsterCharacter* FoundMonster = Cast<AMonsterCharacter>(FoundActors.Last());
            MainGameState->MonsterCharacter = FoundMonster;
        }
    }
}
