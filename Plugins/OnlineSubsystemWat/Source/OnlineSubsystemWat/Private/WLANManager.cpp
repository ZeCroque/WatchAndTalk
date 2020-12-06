#include "WLANManager.h"

#include "NboSerializerWat.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"

FWLANManager::~FWLANManager()
{
	StopWLANProcess();
}

bool FWLANManager::Init()
{	
	if (RelationServerAddr)
	{
		UE_LOG(LogOnline, Warning, TEXT("Socket exists already, initialization skipped."));
		return true;     
	}

	WSADATA WsaData;
	if (!WSAStartup(MAKEWORD(2, 0), &WsaData))
	{
		RelationServerAddr = new sockaddr_in();
		InetPton(AF_INET, L"193.250.68.210", &RelationServerAddr->sin_addr.s_addr);
		RelationServerAddr->sin_family = AF_INET;
		RelationServerAddr->sin_port = htons(7777);
		Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (Socket == INVALID_SOCKET)
		{
			UE_LOG(LogOnline, Error, TEXT("Socket creation failed. Error code:%d"), WSAGetLastError());
			return false;
		}

		// 1 to set non-blocking
		unsigned long Flag = 1;
		if (ioctlsocket(Socket, FIONBIO, &Flag) == SOCKET_ERROR)
		{
			UE_LOG(LogOnline, Error, TEXT("Couldn't set the socket to non-blocking mode. Error code:%d"), WSAGetLastError());
			return false;
		}

		UE_LOG(LogOnline, Warning, TEXT("Socket created."));
		return true;
	}
	UE_LOG(LogOnline, Warning, TEXT("Error:WSA couldn't start"));
	return false;
}

SOCKADDR_IN* FWLANManager::GetHostNATAddress()
{
	if(bIsHost && HostsAddresses)
	{
		return HostsAddresses;
	}
	return nullptr;
}

bool FWLANManager::Host(FOnValidQueryPacketDelegate& QueryDelegate)
{
	bIsHost = true;

	bool bSuccess = true;
	if (!RelationServerAddr)
	{
		bSuccess = Init();
	}
	if (bSuccess)
	{	
		FMessage Message = { MessageType::HostPresentation, {0, 0, {{{0,0,0,0}}}, {0}} };
		if(sendto(Socket, reinterpret_cast<char*>(&Message), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(RelationServerAddr), sizeof(*RelationServerAddr)) == SOCKET_ERROR) 
		{
                  UE_LOG(LogOnline, Warning, TEXT("Sending error. Code:%d"), WSAGetLastError());
                  return false;
		}
		UE_LOG(LogOnline, Warning, TEXT("Presentation message sent."));
		return true;
	}
	return false;
}

bool FWLANManager::Search(const TSharedRef<FOnlineSessionSearch>& SearchSettings, FOnValidResponsePacketDelegate& ResponseDelegate, FOnSearchingTimeoutDelegate& TimeoutDelegate)
{
	bIsHost = false;

    bool bSuccess = false;
        
	if (!RelationServerAddr)
	{
		bSuccess = Init();
	}

	FMessage Message{ MessageType::ClientPresentation, {0, 0, {{{0,0,0,0}}}, {0}} };
	if(sendto(Socket, reinterpret_cast<char*>(&Message), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(RelationServerAddr), sizeof(*RelationServerAddr)) == SOCKET_ERROR)
	{
	    UE_LOG(LogOnline, Warning, TEXT("Sending error. Code:%d"), WSAGetLastError());
	    return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Presentation message sent."));

    if(bSuccess)
    {
    	ClientSessionSearch = &SearchSettings.Get();
	AddOnValidResponsePacketDelegate_Handle(ResponseDelegate);
	AddOnSearchingTimeoutDelegate_Handle(TimeoutDelegate);
    }
    return bIsHost;
}

bool FWLANManager::SendNatPunchThroughPacket(SOCKADDR_IN& PeerAddress, char* PunchPacket)
{
	if(sendto(Socket, PunchPacket, NAT_PUNCHING_PACKET_SIZE, 0, reinterpret_cast<SOCKADDR*>(&PeerAddress), sizeof(PeerAddress)) == SOCKET_ERROR) 
	{
	    UE_LOG(LogOnline, Warning, TEXT("Sending error. Code:%d"), WSAGetLastError());
	    return false;
	}
	UE_LOG(LogOnline, Warning, TEXT("Punch packet sent."));

	return true;
}

void FWLANManager::StopWLANProcess()
{
	if (RelationServerAddr)
	{
		delete RelationServerAddr;
		RelationServerAddr = nullptr;
	}
	if(HostsAddresses)
	{
		if(bIsHost)
		{
			delete HostsAddresses;
		}
		else
		{
			delete[] HostsAddresses;
		}
		HostsAddresses = nullptr;
	}
	if(ClientSessionSearch)
	{
		delete ClientSessionSearch;
		ClientSessionSearch = nullptr;
	}
}

void FWLANManager::SetHostSession(FNamedOnlineSession* Session)
{
	this->HostSession = Session;
}

void FWLANManager::AppendSessionToPacket(FNboSerializeToBufferWat& Packet, FOnlineSession* Session)
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
	Packet << (int32)NumAdvertisedProperties;
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			Packet << It.Key();
			Packet << Setting;
			UE_LOG(LogOnline,Warning , TEXT("%s"), *Setting.ToString());
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
	if(RelationServerAddr)
	{
		char* ReceptionBuffer = new char[MaxBufferLength];
		SOCKADDR_IN SenderAddr;
		int SenderAddrSize = sizeof(SenderAddr);
		const int ReceivedSize = recvfrom(Socket, ReceptionBuffer, MaxBufferLength, 0, reinterpret_cast<SOCKADDR*>(&SenderAddr), &SenderAddrSize);
		if (ReceivedSize == sizeof(FMessage))
		{
			FMessage* Message = reinterpret_cast<FMessage*>(ReceptionBuffer);
			if (bIsHost)
			{
				switch (Message->messageType)
				{
				case MessageType::ClientConnectionRequest:
					UE_LOG(LogOnline, Warning, TEXT("Connection request received."));
					SendNatPunchThroughPacket(Message->adr, "one");
					break;
				case MessageType::HostAddress:
				{
					// Setup the host session info				
					HostsAddresses = new SOCKADDR_IN();
					HostsAddresses[0] = Message->adr;
					FOnlineSessionInfoWat* NewSessionInfo = new FOnlineSessionInfoWat();
					NewSessionInfo->Init(HostsAddresses[0]);
					HostSession->SessionInfo = MakeShareable(NewSessionInfo);
						
					char Ip[16];
					inet_ntop(AF_INET, reinterpret_cast<void*>(&HostsAddresses[0].sin_addr), Ip, 16);
					UE_LOG(LogOnline, Warning, TEXT("NAT adress received:%hs:%d"), Ip, ntohs(HostsAddresses[0].sin_port));
					break;
				}
				case MessageType::SessionRequest:
				{
					UE_LOG(LogOnline, Warning, TEXT("Session request received."));
					FNboSerializeToBufferWat Packet; 
				    AppendSessionToPacket(Packet, HostSession);		    
					int PacketLength = Packet.GetByteCount();
					FMessage Answer = { MessageType::SessionSize, {0, 0, {{{0,0,0,0}}}, {0}} };
		                        Answer.adr = *reinterpret_cast<SOCKADDR_IN *>(&PacketLength);
			                sendto(Socket, reinterpret_cast<char*>(&Answer), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(&SenderAddr), SenderAddrSize);
					UE_LOG(LogOnline, Warning, TEXT("Session size sent."));			  
				    sendto(Socket, reinterpret_cast<char*>(Packet.GetRawBuffer(0)), PacketLength, 0,  reinterpret_cast<SOCKADDR*>(&SenderAddr), SenderAddrSize);
					UE_LOG(LogOnline, Warning, TEXT("Session sent:%s"),*HostSession->SessionInfo->ToDebugString());	
					break;
				}
				default:
					UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Type:%d"), static_cast<int>(Message->messageType));
				}
			}
			else
			{
				switch (Message->messageType)
				{
				case MessageType::HostCount:
					ExpectedHostsCount = *reinterpret_cast<int*>(&Message->adr);
					UE_LOG(LogOnline, Warning, TEXT("Host count received: %d"), ExpectedHostsCount);
					break;
				case MessageType::SessionSize:
					ExpectedSessionSize = *reinterpret_cast<int*>(&Message->adr);
					UE_LOG(LogOnline, Warning, TEXT("Session size received: %d"), ExpectedSessionSize);
			        break;
				default:
					UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Type:%d"), static_cast<int>(Message->messageType));
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
				SendNatPunchThroughPacket(SenderAddr, "two");
			}
			UE_LOG(LogOnline, Warning, TEXT("NAT punch through successful."));

			if (!bIsHost)
			{
				FMessage Answer = { MessageType::SessionRequest, {0, 0, {{{0,0,0,0}}}, {0}} };
				sendto(Socket, reinterpret_cast<char*>(&Answer), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(&SenderAddr), sizeof(SenderAddr));
				UE_LOG(LogOnline, Warning, TEXT("Sent session request."));
			}
		}
		else if (ReceivedSize == ExpectedHostsCount * sizeof(SOCKADDR_IN) && ReceivedSize != 0)
		{
			UE_LOG(LogOnline, Warning, TEXT("Hosts addresses received. Hosts list:"));
			HostsAddresses = new SOCKADDR_IN[ExpectedHostsCount];
			for (int i = 0; i < ExpectedHostsCount; i++)
			{
				//TODO connect to everyone
				HostsAddresses[i] = *reinterpret_cast<SOCKADDR_IN*>(ReceptionBuffer + i * sizeof(SOCKADDR_IN));
				char Ip[16];
				inet_ntop(AF_INET, reinterpret_cast<void*>(&HostsAddresses[i].sin_addr), Ip, 16);
				UE_LOG(LogOnline, Warning, TEXT("%hs:%d"), Ip, ntohs(HostsAddresses[i].sin_port));
			}

			FMessage Answer = { MessageType::ClientConnectionRequest, HostsAddresses[0] };
			sendto(Socket, reinterpret_cast<char*>(&Answer), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(RelationServerAddr), sizeof(*RelationServerAddr));
			UE_LOG(LogOnline, Warning, TEXT("Connection request sent to server."));

			SendNatPunchThroughPacket(HostsAddresses[0], "one");
			UE_LOG(LogOnline, Warning, TEXT("NAT Punching package sent to host..."));
			ExpectedHostsCount = 0;
		}
		else if(ReceivedSize == ExpectedSessionSize && ReceivedSize != 0) 
        {
		    ExpectedSessionSize = 0;
		    // Add space in the search results array
		    FOnlineSessionSearchResult* NewResult = new (ClientSessionSearch->SearchResults) FOnlineSessionSearchResult();
		    FOnlineSession* NewSession = &NewResult->Session;
		    // Prepare to read data from the packet
		    FNboSerializeFromBufferWat Packet(reinterpret_cast<uint8*>(ReceptionBuffer), ReceivedSize);
		    ReadSessionFromPacket(Packet, NewSession);
			UE_LOG(LogOnline, Warning, TEXT("Received session:%s"), *NewSession->SessionInfo->ToDebugString());	
            TriggerOnSearchingTimeoutDelegates();
		}
		else if (ReceivedSize > 0)
		{
			UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Length:%d"), ReceivedSize);
		}
		else if (ReceivedSize == -1 && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			UE_LOG(LogOnline, Warning, TEXT("Error: Reception error. Code:%d"), WSAGetLastError());
		}


		delete[] ReceptionBuffer;
	}
}