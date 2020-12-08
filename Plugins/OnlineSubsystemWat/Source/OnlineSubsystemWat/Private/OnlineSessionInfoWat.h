// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystemInterface.h"
#include "OnlineSubsystemWatPackage.h"

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
	FOnlineSessionInfoWat() : SessionId(TEXT("INVALID")){}

	/** 
	 * Initialize a Null session info with the address of this machine
	 * and an id for the session
	 */
	void Init(TSharedPtr<class FInternetAddr> HostAddr);

	/** The ip & port that the host is listening on*/
	TSharedPtr<class FInternetAddr> HostAddr;
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
		return sizeof(uint64) + sizeof(TSharedPtr<class FInternetAddr>);
	}

	virtual bool IsValid() const override
	{
		return SessionId.IsValid() && HostAddr && HostAddr->IsValid();
	}

	virtual FString ToString() const override
	{
		return SessionId.ToString();
	}

	virtual FString ToDebugString() const override
	{
		
		return FString::Printf(TEXT("HostIP: %s SessionId: %s"), *HostAddr->ToString(true), *SessionId.ToDebugString());
	}

	virtual const FUniqueNetId& GetSessionId() const override
	{
		return SessionId;
	}
};
