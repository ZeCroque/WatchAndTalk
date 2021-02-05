// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WatInputSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig)
class WAT_API UWatInputSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, Category=InputSettings, EditAnywhere, meta=(AllowedClasses="Texture"))
	TMap<FKey, FSoftObjectPath> KeysVisuals;
	
};
