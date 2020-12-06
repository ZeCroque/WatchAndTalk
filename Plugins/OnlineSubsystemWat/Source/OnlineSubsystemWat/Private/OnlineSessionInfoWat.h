// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystemInterface.h"
#include "OnlineSubsystemWatPackage.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#include "WS2tcpip.h"
#include "iphlpapi.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

class FOnlineSubsystemWaT;

// from OnlineSubsystemTypes.h
TEMP_UNIQUENETIDSTRING_SUBCLASS(FUniqueNetIdWat, WAT_SUBSYSTEM);

/** 
 * Implementation of session information
 */
class FOnlineSessionInfoWat : public FOnlineSessionInfo
{
protected:
	
	/** Hidden on purpose */
	FOnlineSessionInfoWat(const FOnlineSessionInfoWat& Src)
	{
	}

	/** Hidden on purpose */
	FOnlineSessionInfoWat& operator=(const FOnlineSessionInfoWat& Src)
	{
		return *this;
	}

PACKAGE_SCOPE:

	/** Constructor */
	FOnlineSessionInfoWat() : HostAddr({0, 0, {{{0,0,0,0}}}, {0}}), SessionId(TEXT("INVALID")){}

	/** 
	 * Initialize a Null session info with the address of this machine
	 * and an id for the session
	 */
	void Init(const SOCKADDR_IN& HostAddr);

	/** The ip & port that the host is listening on*/
	SOCKADDR_IN HostAddr;
	FUniqueNetIdWat SessionId;

public:

	virtual ~FOnlineSessionInfoWat(){}

 	bool operator==(const FOnlineSessionInfoWat& Other) const
 	{
 		return false;
 	}

	virtual const uint8* GetBytes() const override
	{
		return nullptr;
	}

	virtual int32 GetSize() const override
	{
		return sizeof(uint64) + sizeof(SOCKADDR_IN);
	}

	virtual bool IsValid() const override
	{
		return true;
	}

	virtual FString ToString() const override
	{
		return SessionId.ToString();
	}

	virtual FString ToDebugString() const override
	{
		SOCKADDR_IN Tmp = HostAddr;
		char Ip[16];
		inet_ntop(AF_INET, reinterpret_cast<void*>(&Tmp.sin_addr), Ip, 16);	
		return FString::Printf(TEXT("HostIP: %hs:%d SessionId: %s"), Ip, ntohs(HostAddr.sin_port), *SessionId.ToDebugString());
	}

	virtual const FUniqueNetId& GetSessionId() const override
	{
		return SessionId;
	}
};
