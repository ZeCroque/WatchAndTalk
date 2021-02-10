// Fill out your copyright notice in the Description page of Project Settings.

#include "wat/Core/WatGameMode.h"
#include "EngineUtils.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerController.h"


AWatGameMode::AWatGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/Blueprints/Characters/PlayerCharacterBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AWatPlayerController::StaticClass();

	bUseSeamlessTravel = true;
}

