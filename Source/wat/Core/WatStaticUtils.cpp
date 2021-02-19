// Fill out your copyright notice in the Description page of Project Settings.


#include "WatStaticUtils.h"
#include "WatSettings.h"
#include "Engine/AssetManager.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"

UTexture2D* UWatStaticUtils::GetKeyIcon(FName ActionMappingName, bool bIsGamePad)
{
	const auto* InputSettingsWat = GetDefault<UWatSettings>();
	const FSoftObjectPath* InputTexturePath = InputSettingsWat->KeysVisuals.Find(GetKeyForDevice(ActionMappingName, bIsGamePad));
	return Cast<UTexture2D>(InputTexturePath->TryLoad());
}

FKey UWatStaticUtils::GetKeyForDevice(FName ActionMappingName, bool bIsGamePad)
{
	TArray<FInputActionKeyMapping> ActionsMappings;
    UInputSettings::GetInputSettings()->GetActionMappingByName(ActionMappingName, ActionsMappings);

	for(auto ActionMapping : ActionsMappings)
	{
		if(!bIsGamePad && !ActionMapping.Key.IsGamepadKey())
		{
			return ActionMapping.Key;
		}
		if(bIsGamePad && ActionMapping.Key.IsGamepadKey())
		{
			return ActionMapping.Key;
		}
	}
	return FKey();
}
