#include "WLANManager.h"

#include "NboSerializerWat.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IpNetDriverWat.h"

FWLANManager::~FWLANManager()
{
	StopWLANProcess();
}

bool FWLANManager::Init()
{
	if (Socket)
	{
		UE_LOG(LogOnline, Warning, TEXT("Socket exists already, initialization skipped."));
		return true;
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	// Now the listen address
	RelationServerAddress = SocketSubsystem->CreateInternetAddr();
        const auto ResolveInfo = SocketSubsystem->GetHostByName("zecroque.hopto.org");
        while (!ResolveInfo->IsComplete());

        bool bSuccess = (ResolveInfo->GetErrorCode() == 0);

        if (!bSuccess)
        {
            UE_LOG(LogOnline, Warning, TEXT("Address creation failed."));
            return bSuccess;
        }

        auto& ReadAddr = ResolveInfo->GetResolvedAddress();
        uint32 ReadIp = 0;
        ReadAddr.GetIp(ReadIp);
        RelationServerAddress->SetIp(ReadIp);
	RelationServerAddress->SetPort(7777);

	// Now create and set up our sockets (no VDP)
	Socket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("MainSocket"), RelationServerAddress->GetProtocolType());
	if (Socket != nullptr)
	{
		Socket->SetReuseAddr();
		Socket->SetNonBlocking();
		Socket->SetRecvErr();
		bSuccess = Socket->SetBroadcast();
	}
	else
	{
		UE_LOG_ONLINE(Error, TEXT("Failed to create listen socket for LAN beacon"));
	}
	return bSuccess && Socket;
}

int FWLANManager::GetPort() const
{
	return Port;
}

bool FWLANManager::SendPresentationMessage()
{
	bool bSuccess = false;
	if (!Socket)
	{
		bSuccess = Init();
	}
	int SentBytes = 0;
	if (bSuccess)
	{
		FMessage PresentationMessage = { bIsHost ? EWatMessageType::HostPresentation : EWatMessageType::ClientPresentation, {{0},{0}, {0}} };

		bSuccess = Socket->SendTo(reinterpret_cast<uint8*>(&PresentationMessage), sizeof(FMessage), SentBytes, *RelationServerAddress);
		if (!bSuccess)
		{
			ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
			UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
			return false;
		}
		UE_LOG(LogOnline, Warning, TEXT("Sent presentation message : %d bytes to %s"), SentBytes, *RelationServerAddress->ToString(true));
		Port = Socket->GetPortNo();
		return true;
	}
	return false;
}

bool FWLANManager::SendSession(class FNamedOnlineSession* HostSession, FInternetAddr* ClientAddress)
{
	FNboSerializeToBufferWat Packet;
	AppendSessionToPacket(Packet, HostSession);
	int PacketLength = Packet.GetByteCount();

	FMessage Answer = { EWatMessageType::SessionSize, {{0}, {0}, {0}, {0}} };
	Answer.BSDAddr = *reinterpret_cast<FBSDAddress*>(&PacketLength);

	int SentSize = 0;
	if (!Socket->SendTo(reinterpret_cast<uint8*>(&Answer), sizeof(FMessage), SentSize, *ClientAddress))
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
		return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Session size sent."));

	if (!Socket->SendTo(Packet.GetRawBuffer(0), PacketLength, SentSize, *ClientAddress))
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
		return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Session sent:%s"), *HostSession->SessionInfo->ToDebugString());
	return true;
}

bool FWLANManager::SendConnectionRequest()
{
	if(CurrentHostIndex == HostsAddresses.Num())
	{
		StopSearch();
		return true;
	}
	
	FMessage Answer = { EWatMessageType::ClientConnectionRequest, {{0},{0},{0},{0}} };
	WriteAddressIntoMessage(Answer, HostsAddresses[CurrentHostIndex].Get());
	int SentSize = 0;
	if (!Socket->SendTo(reinterpret_cast<uint8*>(&Answer), sizeof(FMessage), SentSize, *RelationServerAddress))
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
		return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Connection request sent to server."));

	if(!SendNatPunchThroughPacket(HostsAddresses[CurrentHostIndex].Get(), reinterpret_cast<uint8*>("one")))
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
		return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("NAT Punching package sent to host..."));
	
	return true;
}

bool FWLANManager::SendHostDisconnectedMessage()
{
    FMessage Answer = { EWatMessageType::HostDisconnected, {{0},{0},{0},{0}} };
    WriteAddressIntoMessage(Answer, HostsAddresses[CurrentHostIndex].Get());
    int SentSize = 0;
    if (!Socket->SendTo(reinterpret_cast<uint8*>(&Answer), sizeof(FMessage), SentSize, *RelationServerAddress))
    {
	    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	    UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
	    return false;
    }
    UE_LOG(LogOnline, Warning, TEXT("Connection request sent to server."));
    return true;
}

bool FWLANManager::SendJoinRequest(FInternetAddr* HostAddress)
{
	FMessage PresentationMessage = {EWatMessageType::JoinRequest, {{0},{0}, {0}} };
	int SentBytes;
	if (!Socket->SendTo(reinterpret_cast<uint8*>(&PresentationMessage), sizeof(FMessage), SentBytes, *HostAddress))
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		UE_LOG(LogOnline, Warning, TEXT("Send error. Code:%d"), SocketSubsystem->GetLastErrorCode());
		return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Sent join request : %d bytes to %s"), SentBytes, *HostAddress->ToString(true));
        return true;
}

void FWLANManager::StopSearch()
{
	HostsSearchTimeoutDuration = HOSTS_SEARCH_TIMEOUT_DURATION;
	bIsSearching = false;
	CurrentHostIndex = 0;
	TriggerOnSearchingTimeoutDelegates();
}

void FWLANManager::ReadAddressFromBSDAddress(FBSDAddress BSDAddress, FInternetAddr* Address) const
{
	Algo::Reverse(BSDAddress.PortBytes, sizeof(uint16));
	Algo::Reverse(BSDAddress.IpBytes, sizeof(uint32));

	Address->SetIp(*reinterpret_cast<uint32*>(BSDAddress.IpBytes));
	Address->SetPort(*reinterpret_cast<uint16*>(BSDAddress.PortBytes));
}

void FWLANManager::ReadAddressFromMessage(FMessage Message, FInternetAddr* Address) const
{
	ReadAddressFromBSDAddress(Message.BSDAddr, Address);
}

void FWLANManager::WriteAddressIntoMessage(FMessage& Message, FInternetAddr* Address) const
{
	*reinterpret_cast<uint16*>(Message.BSDAddr.PortBytes) = Address->GetPort();
	Address->GetIp(*reinterpret_cast<uint32*>(Message.BSDAddr.IpBytes));
	Algo::Reverse(Message.BSDAddr.PortBytes, sizeof(uint16));
	Algo::Reverse(Message.BSDAddr.IpBytes, sizeof(uint32));
}

bool FWLANManager::Host(FOnSessionStartedDelegate& SessionStartedDelegate, FOnValidSessionRequestDelegate& SessionRequestDelegate, FOnJoinRequestDelegate& JoinRequestDelegate)
{
	bIsHost = true;
	AddOnSessionStartedDelegate_Handle(SessionStartedDelegate);
	AddOnValidSessionRequestDelegate_Handle(SessionRequestDelegate);
	AddOnJoinRequestDelegate_Handle(JoinRequestDelegate);
	return SendPresentationMessage();
}

bool FWLANManager::Search(const TSharedRef<FOnlineSessionSearch>& SearchSettings, FOnSearchingTimeoutDelegate& TimeoutDelegate)
{
	bIsHost = false;

	if (SendPresentationMessage())
	{
		ClientSessionSearch = SearchSettings;
		AddOnSearchingTimeoutDelegate_Handle(TimeoutDelegate);
		return true;
	}
	return false;
}

bool FWLANManager::SendNatPunchThroughPacket(FInternetAddr* PeerAddress, uint8* PunchPacket)
{
	int SentSize = 0;
	if (!Socket->SendTo(PunchPacket, NAT_PUNCHING_PACKET_SIZE, SentSize, *PeerAddress))
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		UE_LOG(LogOnline, Warning, TEXT("Sending error. Code:%d"), SocketSubsystem->GetLastErrorCode());
		return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Punch packet sent."));

	return true;
}

void FWLANManager::StopWLANProcess()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	SocketSubsystem->DestroySocket(Socket);
	Socket = nullptr;

	if(!bIsHost)
	{
		HostsAddresses.Empty();
	}

	ClientSessionSearch.Reset();
}

void FWLANManager::AppendSearchSettingsToPacket(FNboSerializeToBufferWat& Packet, FOnlineKeyValuePairs<FName, FOnlineSessionSearchParam>& SearchParams)
{
	Packet << SearchParams.Num();
	for (auto SearchParam : SearchParams)
	{
		Packet << SearchParam.Key;
		Packet << *reinterpret_cast<FOnlineSessionSetting*>(&SearchParam.Value);
	}
}

void FWLANManager::AppendSessionToPacket(FNboSerializeToBufferWat& Packet, FOnlineSession* Session) const
{
	/** Owner of the session */
	Packet << *StaticCastSharedPtr<const FUniqueNetIdWat>(Session->OwningUserId)
		<< Session->OwningUserName
		<< Session->NumOpenPrivateConnections
		<< Session->NumOpenPublicConnections;

	// Write host info (host addr, session id, and key)
	Packet << *StaticCastSharedPtr<FOnlineSessionInfoWat>(Session->SessionInfo);

	// Now append per game settings
	AppendSessionSettingsToPacket(Packet, &Session->SessionSettings);
}

void FWLANManager::AppendSessionSettingsToPacket(FNboSerializeToBufferWat& Packet, FOnlineSessionSettings* SessionSettings)
{
	UE_LOG_ONLINE_SESSION(Verbose, TEXT("Sending session settings to client"));

	// Members of the session settings class
	Packet << SessionSettings->NumPublicConnections
		<< SessionSettings->NumPrivateConnections
		<< (uint8)SessionSettings->bShouldAdvertise
		<< (uint8)SessionSettings->bIsLANMatch
		<< (uint8)SessionSettings->bIsDedicated
		<< (uint8)SessionSettings->bUsesStats
		<< (uint8)SessionSettings->bAllowJoinInProgress
		<< (uint8)SessionSettings->bAllowInvites
		<< (uint8)SessionSettings->bUsesPresence
		<< (uint8)SessionSettings->bAllowJoinViaPresence
		<< (uint8)SessionSettings->bAllowJoinViaPresenceFriendsOnly
		<< (uint8)SessionSettings->bAntiCheatProtected
		<< SessionSettings->BuildUniqueId;

	// First count number of advertised keys
	int32 NumAdvertisedProperties = 0;
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			NumAdvertisedProperties++;
		}
	}

	// Add count of advertised keys and the data
	Packet << static_cast<int32>(NumAdvertisedProperties);
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			Packet << It.Key();
			Packet << Setting;
			UE_LOG(LogOnline, Warning, TEXT("%s"), *Setting.ToString());
		}
	}

}

void FWLANManager::ReadSearchSettingsFromPacket(FNboSerializeFromBufferWat& Packet, FOnlineSearchSettings& SearchSettings)
{
	int32 length = 0;
	Packet >> length;

	for (int32 i = 0; i < length; ++i)
	{
		FName Key;
		Packet >> Key;

		FVariantData Data;
		Packet >> Data;
		int32 ComparisonOp = 0;
		Packet >> ComparisonOp;
		int32 ID;
		Packet >> ID;

		const auto Type = Data.GetType();
		switch (Type)
		{
		case EOnlineKeyValuePairDataType::Empty:
			UE_LOG(LogOnline, Warning, TEXT("Value was empty, skipping."), Type);
			break;
		case EOnlineKeyValuePairDataType::Int32:
		{
			int32 Value = 0;
			Data.GetValue(Value);
			SearchSettings.Set(SEARCH_PRESENCE, Value, static_cast<EOnlineComparisonOp::Type>(ComparisonOp), ID);
			break;
		}
		case EOnlineKeyValuePairDataType::Int64:
		{
			uint64 Value = 0;
			Data.GetValue(Value);
			SearchSettings.Set(SEARCH_PRESENCE, Value, static_cast<EOnlineComparisonOp::Type>(ComparisonOp), ID);
			break;
		}
		case EOnlineKeyValuePairDataType::Double:
		{
			double Value = 0;
			Data.GetValue(Value);
			SearchSettings.Set(SEARCH_PRESENCE, Value, static_cast<EOnlineComparisonOp::Type>(ComparisonOp), ID);
			break;
		}
		case EOnlineKeyValuePairDataType::String:
		{
			FString Value;
			Data.GetValue(Value);
			SearchSettings.Set(SEARCH_PRESENCE, Value, static_cast<EOnlineComparisonOp::Type>(ComparisonOp), ID);
			break;
		}
		case EOnlineKeyValuePairDataType::Float:
		{
			float Value;
			Data.GetValue(Value);
			SearchSettings.Set(SEARCH_PRESENCE, Value, static_cast<EOnlineComparisonOp::Type>(ComparisonOp), ID);
			break;
		}
		case EOnlineKeyValuePairDataType::Bool:
		{
			bool Value;
			Data.GetValue(Value);
			SearchSettings.Set(SEARCH_PRESENCE, Value, static_cast<EOnlineComparisonOp::Type>(ComparisonOp), ID);
			break;
		}
		default:
			UE_LOG(LogOnline, Warning, TEXT("Unsupported EOnlineKeyValuePairDataType: %d"), Type);
			break;
		}
	}
}

void FWLANManager::ReadSessionFromPacket(FNboSerializeFromBufferWat& Packet, FOnlineSession* Session)
{
	UE_LOG_ONLINE_SESSION(Verbose, TEXT("Reading session information from server"));

	/** Owner of the session */
	FUniqueNetIdWat* UniqueId = new FUniqueNetIdWat;
	Packet >> *UniqueId
		>> Session->OwningUserName
		>> Session->NumOpenPrivateConnections
		>> Session->NumOpenPublicConnections;

	Session->OwningUserId = MakeShareable(UniqueId);

	// Allocate and read the connection data
	FOnlineSessionInfoWat* WatSessionInfo = new FOnlineSessionInfoWat();
	WatSessionInfo->HostAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Packet >> *WatSessionInfo;
	Session->SessionInfo = MakeShareable(WatSessionInfo);

	// Read any per object data using the server object
	ReadSettingsFromPacket(Packet, Session->SessionSettings);
}

void FWLANManager::ReadSettingsFromPacket(FNboSerializeFromBufferWat& Packet, FOnlineSessionSettings& SessionSettings)
{
	UE_LOG_ONLINE_SESSION(Verbose, TEXT("Reading game settings from server"));

	// Clear out any old settings
	SessionSettings.Settings.Empty();

	// Members of the session settings class
	Packet >> SessionSettings.NumPublicConnections
		>> SessionSettings.NumPrivateConnections;
	uint8 Read = 0;
	// Read all the bools as bytes
	Packet >> Read;
	SessionSettings.bShouldAdvertise = !!Read;
	Packet >> Read;
	SessionSettings.bIsLANMatch = !!Read;
	Packet >> Read;
	SessionSettings.bIsDedicated = !!Read;
	Packet >> Read;
	SessionSettings.bUsesStats = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinInProgress = !!Read;
	Packet >> Read;
	SessionSettings.bAllowInvites = !!Read;
	Packet >> Read;
	SessionSettings.bUsesPresence = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinViaPresence = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = !!Read;
	Packet >> Read;
	SessionSettings.bAntiCheatProtected = !!Read;

	// BuildId
	Packet >> SessionSettings.BuildUniqueId;

	// Now read the contexts and properties from the settings class
	int32 NumAdvertisedProperties = 0;
	// First, read the number of advertised properties involved, so we can presize the array
	Packet >> NumAdvertisedProperties;
	if (Packet.HasOverflow() == false)
	{
		FName Key;
		// Now read each context individually
		for (int32 Index = 0;
			Index < NumAdvertisedProperties && Packet.HasOverflow() == false;
			Index++)
		{
			FOnlineSessionSetting Setting;
			Packet >> Key;
			Packet >> Setting;
			SessionSettings.Set(Key, Setting);
			UE_LOG(LogOnline, Warning, TEXT("%s"), *Setting.ToString());
		}
	}

	// If there was an overflow, treat the string settings/properties as broken
	if (Packet.HasOverflow())
	{
		SessionSettings.Settings.Empty();
		UE_LOG_ONLINE_SESSION(Verbose, TEXT("Packet overflow detected in ReadGameSettingsFromPacket()"));
	}
}

void FWLANManager::Tick(float DeltaTime)
{
	if (Socket)
	{
		uint8* ReceptionBuffer = new uint8[MaxBufferLength];
		int ReceivedSize = 0;
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		const auto SenderAddress = SocketSubsystem->CreateInternetAddr();

		if (Socket->RecvFrom(ReceptionBuffer, MaxBufferLength, ReceivedSize, *SenderAddress))
		{
			if (ReceivedSize > 0)
			{
				UE_LOG(LogOnline, Warning, TEXT("Received %d bytes from %s"), ReceivedSize, *SenderAddress->ToString(true));
				if (ReceivedSize == sizeof(FMessage))
				{
					FMessage* Message = reinterpret_cast<FMessage*>(ReceptionBuffer);
					if (bIsHost)
					{
						switch (Message->MessageType)
						{
						case EWatMessageType::ClientConnectionRequest:
						{
							UE_LOG(LogOnline, Warning, TEXT("Connection request received."));
							const auto Address = SocketSubsystem->CreateInternetAddr();
							ReadAddressFromMessage(*Message, &Address.Get());
							SendNatPunchThroughPacket(&Address.Get(), reinterpret_cast<uint8*>("one"));
							break;
						}
						case EWatMessageType::HostAddress:
						{
							HostsAddresses.Add(SocketSubsystem->CreateInternetAddr());							
                                                        ReadAddressFromMessage(*Message, HostsAddresses[0].Get());						
							UE_LOG(LogOnline, Warning, TEXT("NAT adress received:%s"), *HostsAddresses[0]->ToString(true));
							TriggerOnSessionStartedDelegates(HostsAddresses[0]);
							break;
						}
						case EWatMessageType::SessionRequest:
						{
							ExpectedSessionSettingsSize = *reinterpret_cast<int*>(&Message->BSDAddr);
							UE_LOG(LogOnline, Warning, TEXT("Session settings size received: %d"), ExpectedSessionSettingsSize);
							break;
						}
						case EWatMessageType::JoinRequest:
							UE_LOG(LogOnline, Warning, TEXT("Join request received."));
		                                        SendHostDisconnectedMessage();
							StopWLANProcess();
							TriggerOnJoinRequestDelegates();
							break;
						default:
							UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Type:%d"), static_cast<int>(Message->MessageType));
						}
					}
					else
					{
						switch (Message->MessageType)
						{
						case EWatMessageType::HostCount:
							ExpectedHostsCount = *reinterpret_cast<int*>(&Message->BSDAddr);
							UE_LOG(LogOnline, Warning, TEXT("Host count received: %d"), ExpectedHostsCount);
							if(ExpectedHostsCount == 0) 
							{
		                                                StopWLANProcess();
							        TriggerOnSearchingTimeoutDelegates();
							}
							break;
						case EWatMessageType::SessionSize:
							ExpectedSessionSize = *reinterpret_cast<int*>(&Message->BSDAddr);
							UE_LOG(LogOnline, Warning, TEXT("Session size received: %d"), ExpectedSessionSize);
							break;
						default:
							UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Type:%d"), static_cast<int>(Message->MessageType));
						}
					}
				}
				else if (ReceivedSize == NAT_PUNCHING_PACKET_SIZE)
				{
					char ReceivedPunchPacked[4] = { '\0' };
					memcpy(ReceivedPunchPacked, ReceptionBuffer, NAT_PUNCHING_PACKET_SIZE);
					if (!strcmp(ReceivedPunchPacked, "one"))
					{
						UE_LOG(LogOnline, Warning, TEXT("Peer punched our NAT first. Trying to punch his again..."));					
						SendNatPunchThroughPacket(&SenderAddress.Get(), reinterpret_cast<uint8*>("two"));
					}
					UE_LOG(LogOnline, Warning, TEXT("NAT punch through successful."));

					if (!bIsHost)
					{
						FNboSerializeToBufferWat Packet;
						AppendSearchSettingsToPacket(Packet, ClientSessionSearch->QuerySettings.SearchParams);
						int PacketLength = Packet.GetByteCount();

						FMessage Answer = { EWatMessageType::SessionRequest, {{0}, {0}, {0}, {0}} };
						Answer.BSDAddr = *reinterpret_cast<FBSDAddress*>(&PacketLength);
						int SentSize = 0;
						Socket->SendTo(reinterpret_cast<uint8*>(&Answer), sizeof(FMessage), SentSize, *SenderAddress);
						UE_LOG(LogOnline, Warning, TEXT("Sent session settings size."));

						Socket->SendTo(Packet.GetRawBuffer(0), PacketLength, SentSize, *SenderAddress);
						UE_LOG(LogOnline, Warning, TEXT("Sent sessions settings."));
					}
				}
				else if (ReceivedSize == ExpectedHostsCount * sizeof(FBSDAddress) && ReceivedSize != 0)
				{
					UE_LOG(LogOnline, Warning, TEXT("Hosts addresses received. Hosts list:"));
					HostsAddresses.Reserve(ExpectedHostsCount);
					HostsAddresses.SetNum(ExpectedHostsCount);
					for (int i = 0; i < ExpectedHostsCount; i++)
					{
						HostsAddresses[i] = SocketSubsystem->CreateInternetAddr();
						ReadAddressFromBSDAddress(*reinterpret_cast<FBSDAddress*>(ReceptionBuffer + i * sizeof(FBSDAddress)), HostsAddresses[i].Get());
						UE_LOG(LogOnline, Warning, TEXT("%s"), *HostsAddresses[i]->ToString(true));
					}
					ExpectedHostsCount = 0;
					SendConnectionRequest();
					bIsSearching = true;
				}
				else if (ReceivedSize == ExpectedSessionSize && ReceivedSize != 0)
				{
					ExpectedSessionSize = 0;
					// Add space in the search results array
					FOnlineSessionSearchResult* NewResult = new (ClientSessionSearch->SearchResults) FOnlineSessionSearchResult();
					FOnlineSession* NewSession = &NewResult->Session;
					// Prepare to read data from the packet
					FNboSerializeFromBufferWat Packet(reinterpret_cast<uint8*>(ReceptionBuffer), ReceivedSize);
					ReadSessionFromPacket(Packet, NewSession);
					UE_LOG(LogOnline, Warning, TEXT("Received session:%s"), *NewSession->SessionInfo->ToDebugString());
					++CurrentHostIndex;
					SendConnectionRequest();
				}
				else if (ReceivedSize == ExpectedSessionSettingsSize && ReceivedSize != 0)
				{
					ExpectedSessionSettingsSize = 0;
					FNboSerializeFromBufferWat Packet(reinterpret_cast<uint8*>(ReceptionBuffer), ReceivedSize);
					FOnlineSearchSettings OnlineSearchSettings;
					ReadSearchSettingsFromPacket(Packet, OnlineSearchSettings);

					TriggerOnValidSessionRequestDelegates(&OnlineSearchSettings, &SenderAddress.Get());
				}
				else
				{
					UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Length:%d"), ReceivedSize);
				}
			}
			else
			{
				UE_LOG(LogOnline, Warning, TEXT("Error: Reception error. Code:%d"), SocketSubsystem->GetLastErrorCode());
			}
		}
		if(bIsSearching)
		{
			HostConnectionTimeoutDuration -= DeltaTime;
			HostsSearchTimeoutDuration -= DeltaTime;
			if(HostConnectionTimeoutDuration <= 0.f)
			{
				HostConnectionTimeoutDuration = HOST_CONNECTION_TIMEOUT_DURATION;
				SendHostDisconnectedMessage();
				++CurrentHostIndex;
				SendConnectionRequest();
			}
			if(HostsSearchTimeoutDuration <= 0.f)
			{
				StopSearch();
			}
		}
		delete[] ReceptionBuffer;
	}
}