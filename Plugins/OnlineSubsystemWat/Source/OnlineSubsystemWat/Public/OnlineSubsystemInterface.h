#pragma once
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemWatPackage.h"

#ifndef WAT_SUBSYSTEM
#define WAT_SUBSYSTEM FName(TEXT("WAT"))
#endif

class FOnlineSessionWat;

/** Forward declarations of all interface classes */
typedef TSharedPtr<class FOnlineSessionWat, ESPMode::ThreadSafe> FOnlineSessionWatPtr;
typedef TSharedPtr<class FOnlineIdentityWat, ESPMode::ThreadSafe> FOnlineIdentityWatPtr;

/**
 *	OnlineSubsystemWat - Implementation of the online subsystem for Wat services
 */
class ONLINESUBSYSTEMWAT_API FOnlineSubsystemWaT : public FOnlineSubsystemImpl
{
public:


	virtual ~FOnlineSubsystemWaT() = default;

	// IOnlineSubsystem
	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlineStorePtr GetStoreInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;

	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

    int GetPort() const;
	
	// FTickerObjectBase
	virtual bool Tick(float DeltaTime) override;

	PACKAGE_SCOPE:
	// FOnlineSubsystem

	/** Only the factory makes instances */
	FOnlineSubsystemWaT() = delete;
	FOnlineSubsystemWaT(FName InInstanceName) :
		FOnlineSubsystemImpl(WAT_SUBSYSTEM, InInstanceName),
		SessionInterface(nullptr),
		IdentityInterface(nullptr)
	{}
	

private:

	/** Interface to the session services */
	FOnlineSessionWatPtr SessionInterface;

	/** Interface to the identity registration/auth services */
	FOnlineIdentityWatPtr IdentityInterface;
};

typedef TSharedPtr<FOnlineSubsystemWaT, ESPMode::ThreadSafe> FOnlineSubsystemWatPtr;

