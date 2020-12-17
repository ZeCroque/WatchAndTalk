// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainLevelScriptActor.h"
#include "SecurityCamera.h"
#include "Characters/MonsterCharacter.h"
#include "Interactables/EndGameInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Core/MainGameState.h"


class AMainGameState;
void AMainLevelScriptActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    auto* GameState = GetWorld()->GetGameState();
    AMainGameState* MainGameState = Cast<AMainGameState>(GameState);

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASecurityCamera::StaticClass(), FoundActors);

    if (FoundActors.Num() != 0)
    {
        MainGameState->Player1Cameras.Reserve(FoundActors.Num());
        MainGameState->Player1Cameras.SetNum(FoundActors.Num());
        for (auto* FoundActor : FoundActors)
        {
            auto* FoundCameraActor = Cast<ASecurityCamera>(FoundActor);
            if (FoundCameraActor)
            {
                MainGameState->Player1Cameras[FoundCameraActor->ZoneNumber].ZoneCameras.Add(FoundCameraActor);
            }
        }
    }
    TArray<AActor*> FoundEndGameInteractables;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEndGameInteractable::StaticClass(), FoundEndGameInteractables);
    if (FoundEndGameInteractables.Num() != 0)
    {
        MainGameState->EndGameInteractables.Reserve(FoundEndGameInteractables.Num());
        MainGameState->EndGameInteractables.SetNum(FoundEndGameInteractables.Num());
        for (AActor* FoundEndGameInteractable : FoundEndGameInteractables)
        {
            AEndGameInteractable* Interactable = Cast<AEndGameInteractable>(FoundEndGameInteractable);
            if (Interactable)
            {
                MainGameState->EndGameInteractables.Add(Interactable);
            }
        }
    }

    TArray<AActor*> FoundMonster;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterCharacter::StaticClass(), FoundMonster);
    if (FoundMonster.Num() == 1)
    {
        AMonsterCharacter* Monster = Cast<AMonsterCharacter>(FoundMonster.Last());
        MainGameState->MonsterCharacter = Monster;
    }
    
    
    //TODO if(Camera->GetLevel() == Player1Level)
}
