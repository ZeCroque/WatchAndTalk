// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemWat.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemInterface.h"

#define LOCTEXT_NAMESPACE "FOnlineSubsystemWatModule"

class FOnlineFactoryWat : public IOnlineFactory
{
public:

	FOnlineFactoryWat() {}
	virtual ~FOnlineFactoryWat() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override
	{
		FOnlineSubsystemWatPtr OnlineSub = MakeShared<FOnlineSubsystemWaT, ESPMode::ThreadSafe>(InstanceName);
		if (OnlineSub->IsEnabled())
		{
			if(!OnlineSub->Init())
			{
				UE_LOG_ONLINE(Warning, TEXT("Wat API failed to initialize!"));
				OnlineSub->Shutdown();
				OnlineSub = nullptr;
			}
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("Wat API disabled!"));
			OnlineSub->Shutdown();
			OnlineSub = nullptr;
		}

		return OnlineSub;
	}
};

void FOnlineSubsystemWatModule::StartupModule()
{
	WatFactory = new FOnlineFactoryWat();
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(WAT_SUBSYSTEM, WatFactory);
}

void FOnlineSubsystemWatModule::ShutdownModule()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(WAT_SUBSYSTEM);
	
	delete WatFactory;
	WatFactory = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOnlineSubsystemWatModule, OnlineSubsystemWat)