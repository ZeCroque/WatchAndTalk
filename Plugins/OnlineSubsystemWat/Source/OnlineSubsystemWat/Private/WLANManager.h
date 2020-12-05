#pragma once

#include "CoreMinimal.h"
#include "OnlineDelegateMacros.h"

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

#define NAT_PUNCHING_PACKET_SIZE 4
#define MAX_HOST_COUNT_DEFAULT 25
#define MAX_BUFFER_LENGTH_DEFAULT MAX_HOST_COUNT_DEFAULT*sizeof(SOCKADDR_IN)

enum class MessageType {Error = 0, HostPresentation = 1, ClientPresentation = 2, HostCount = 2, ClientConnectionRequest = 3, HostAddress = 4, SessionRequest = 5, SessionSize = 6};

struct FMessage
{
	MessageType messageType;
	SOCKADDR_IN adr;
};


DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnValidQueryPacket, uint8*, int32, uint64);
typedef FOnValidQueryPacket::FDelegate FOnValidQueryPacketDelegate;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnValidResponsePacket, uint8*, int32);
typedef FOnValidResponsePacket::FDelegate FOnValidResponsePacketDelegate;

DECLARE_MULTICAST_DELEGATE(FOnSearchingTimeout);
typedef FOnSearchingTimeout::FDelegate FOnSearchingTimeoutDelegate;

class FWLANManager final
{
private:
	SOCKADDR_IN* RelationServerAddr;
	SOCKET Socket;
	bool bIsHost;
	int MaxHostCount;
	int MaxBufferLength;
	int ExpectedHostsCount;
	SOCKADDR_IN* HostsAddresses;
	class FOnlineSessionSearch* ClientSessionSearch;
	class FNamedOnlineSession* HostSession;
        int ExpectedSessionSize;
	
public:
	
	FWLANManager() : RelationServerAddr(nullptr), Socket(INVALID_SOCKET), bIsHost(false), MaxHostCount(MAX_HOST_COUNT_DEFAULT), MaxBufferLength(MAX_BUFFER_LENGTH_DEFAULT), ExpectedHostsCount(0), HostsAddresses(nullptr), ClientSessionSearch(nullptr), HostSession(nullptr), ExpectedSessionSize(0){}

	virtual ~FWLANManager();

	//TODO relation server addr & port
	bool Init();

	SOCKADDR_IN* GetHostNATAddress();
	/**
	 * Creates the LAN beacon for queries/advertising servers
	 *
	 * @param QueryDelegate delegate to fire when a client query is received
	 */
	bool Host(FOnValidQueryPacketDelegate& QueryDelegate);

	/**
	 * Creates the LAN beacon for queries/advertising servers
	 *
	 * @param ResponseDelegate delegate to fire when a server response is received
	 * @param TimeoutDelegate delegate to fire if we exceed maximum search time
	 *
	 * @return true if search was started successfully, false otherwise
	 */
	bool Search(const TSharedRef<FOnlineSessionSearch>& SearchSettings, FOnValidResponsePacketDelegate& ResponseDelegate, FOnSearchingTimeoutDelegate& TimeoutDelegate);

	//TODO timeout
	bool SendNatPunchThroughPacket(SOCKADDR_IN& PeerAddress, char* PunchPacket);

	/**
	 * Send the relation server that we're ne longer searching for clients
	 */
	void StopWLANProcess();

	void SetHostSession(class FNamedOnlineSession* Session);

	void AppendSessionToPacket(class FNboSerializeToBufferWat& Packet, class FOnlineSession* Session);

	void AppendSessionSettingsToPacket(class FNboSerializeToBufferWat& Packet, class FOnlineSessionSettings* SessionSettings);

	void ReadSessionFromPacket(class FNboSerializeFromBufferWat& Packet, FOnlineSession* Session);

        void ReadSettingsFromPacket(class FNboSerializeFromBufferWat& Packet, FOnlineSessionSettings& SessionSettings);

	void Tick(float DeltaTime);

	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnValidQueryPacket, uint8*, int32, uint64);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnValidResponsePacket, uint8*, int32);
	DEFINE_ONLINE_DELEGATE(OnSearchingTimeout);
};	