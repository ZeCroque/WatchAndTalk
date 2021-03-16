// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WatStaticUtils.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UWatStaticUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UTexture2D* GetKeyIcon(FName ActionMappingName, bool bIsGamePad);

	static FKey GetKeyForDevice(FName ActionMappingName, bool bIsGamePad);

	static void ShowLoadingScreenOnAllPlayers(UWorld* World);
};
