// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameMode.h"
#include "EngineUtils.h"
#include "Core/MainGameState.h"
#include "Core/MainPlayerController.h"


AMainGameMode::AMainGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/Blueprints/Characters/PlayerCharacterBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AMainPlayerController::StaticClass();

	bUseSeamlessTravel = true;
}

