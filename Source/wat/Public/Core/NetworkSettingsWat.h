// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetworkSettingsWat.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig)
class WAT_API UNetworkSettingsWat : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath LobbyLevel;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath MainLevel;

	UPROPERTY(config, Category=NetworkSettings, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath MainMenuLevel;
};
