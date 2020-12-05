// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionInfoWat.h"
#include "NboSerializer.h"

/**
 * Serializes data in network byte order form into a buffer
 */
class FNboSerializeToBufferWat : public FNboSerializeToBuffer
{
public:
	/** Default constructor zeros num bytes*/
	FNboSerializeToBufferWat() :
		FNboSerializeToBuffer(512)
	{
	}

	/** Constructor specifying the size to use */
	FNboSerializeToBufferWat(uint32 Size) :
		FNboSerializeToBuffer(Size)
	{
	}

	/**
	 * Adds Null session info to the buffer
	 */
 	friend inline FNboSerializeToBufferWat& operator<<(FNboSerializeToBufferWat& Ar, const FOnlineSessionInfoWat& SessionInfo)
 	{
		check(SessionInfo.IsValid());
		// Skip SessionType (assigned at creation)
		Ar << SessionInfo.SessionId;
		Ar << SessionInfo.HostAddr;
		return Ar;
 	}

	/**
	 * Adds Null Unique Id to the buffer
	 */
	friend inline FNboSerializeToBufferWat& operator<<(FNboSerializeToBufferWat& Ar, const FUniqueNetIdWat& UniqueId)
	{
		Ar << UniqueId.UniqueNetIdStr;
		return Ar;
	}

	friend inline FNboSerializeToBufferWat& operator<<(FNboSerializeToBufferWat& Ar, const SOCKADDR_IN& HostAddr)
	{
		SOCKADDR_IN Tmp = HostAddr;
		Ar.WriteBinary(reinterpret_cast<uint8*>(&Tmp), sizeof(SOCKADDR_IN));
		return Ar;
	}
};

/**
 * Class used to write data into packets for sending via system link
 */
class FNboSerializeFromBufferWat : public FNboSerializeFromBuffer
{
public:
	/**
	 * Initializes the buffer, size, and zeros the read offset
	 */
	FNboSerializeFromBufferWat(uint8* Packet,int32 Length) :
		FNboSerializeFromBuffer(Packet,Length)
	{
	}

	/**
	 * Reads Null session info from the buffer
	 */
 	friend inline FNboSerializeFromBufferWat& operator>>(FNboSerializeFromBufferWat& Ar, FOnlineSessionInfoWat& SessionInfo)
 	{
		check(SessionInfo.IsValid());
		// Skip SessionType (assigned at creation)
		Ar >> SessionInfo.SessionId; 
		Ar >> SessionInfo.HostAddr;
		return Ar;
 	}

	/**
	 * Reads Null Unique Id from the buffer
	 */
	friend inline FNboSerializeFromBufferWat& operator>>(FNboSerializeFromBufferWat& Ar, FUniqueNetIdWat& UniqueId)
	{
		Ar >> UniqueId.UniqueNetIdStr;
		return Ar;
	}

	friend inline FNboSerializeFromBufferWat& operator>>(FNboSerializeFromBufferWat& Ar, SOCKADDR_IN& HostAddr)
	{
		FString StringBuffer;
		Ar >> StringBuffer;
		char* Buffer = TCHAR_TO_ANSI(*StringBuffer); 
		memcpy(&HostAddr, Buffer, StringBuffer.Len());
		
		return Ar;
	}
};
