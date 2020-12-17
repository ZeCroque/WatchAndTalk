// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FOnlineSubsystemWatModule : public IModuleInterface
{
private:
	class FOnlineFactoryWat* WatFactory;
	
public:

	/** IModuleInterface implementation */
	FOnlineSubsystemWatModule() : WatFactory()
	{}
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
