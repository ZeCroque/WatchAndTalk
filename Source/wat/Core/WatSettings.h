// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WatSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig)
class WAT_API UWatSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, Category=InputSettings, EditAnywhere, meta=(AllowedClasses="Texture"))
	TMap<FKey, FSoftObjectPath> KeysVisuals;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath LobbyLevel;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath MainLevel;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath TutoLevel;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath MainMenuLevel;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath EasterEggLevel;
};
