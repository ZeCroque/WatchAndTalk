// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"

AMainGameMode::AMainGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Characters/PlayerCharacterBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
