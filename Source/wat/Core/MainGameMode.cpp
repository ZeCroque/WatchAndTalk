// Fill out your copyright notice in the Description page of Project Settings.

#include "wat/Core/MainGameMode.h"
#include "EngineUtils.h"
#include "wat/Core/MainGameState.h"
#include "wat/Core/MainPlayerController.h"


AMainGameMode::AMainGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/Blueprints/Characters/PlayerCharacterBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AMainPlayerController::StaticClass();

	bUseSeamlessTravel = true;
}

