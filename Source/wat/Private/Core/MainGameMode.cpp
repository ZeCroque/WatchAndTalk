// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameMode.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Core/MainGameState.h"
#include "SecurityCamera.h"

AMainGameMode::AMainGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Characters/PlayerCharacterBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void AMainGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	
	//TODO if(Camera->GetLevel() == Player1Level)
}
