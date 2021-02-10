// Fill out your copyright notice in the Description page of Project Settings.


#include "WatStaticUtils.h"
#include "WatInputSettings.h"
#include "Engine/AssetManager.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"

UTexture2D* UWatStaticUtils::GetKeyIcon(FName ActionMappingName, bool bIsGamePad)
{
	const auto* InputSettingsWat = GetDefault<UWatInputSettings>();
	const FSoftObjectPath* InputTexturePath = InputSettingsWat->KeysVisuals.Find(GetKeyForDevice(ActionMappingName, bIsGamePad));
	
	const auto AssetData = UAssetManager::Get().GetAssetRegistry().GetAssetByObjectPath(InputTexturePath->GetAssetPathName());	
	return Cast<UTexture2D>(AssetData.GetAsset());
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
