// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NetworkSettingsWat.generated.h"

/**
 * 
 */
UCLASS(Config=NetworkSettings)
class WAT_API UNetworkSettingsWat : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath LobbyLevel;

	UPROPERTY(config, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath MainLevel;

	UPROPERTY(config, EditAnywhere, meta=(AllowedClasses="World"))
	FSoftObjectPath MainMenuLevel;
};
