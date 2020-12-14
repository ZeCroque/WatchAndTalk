#pragma once

#include "CoreMinimal.h"

#include "OnlineDelegateMacros.h"
#include "OnlineKeyValuePair.h"

#define NAT_PUNCHING_PACKET_SIZE 4
#define MAX_HOST_COUNT_DEFAULT 25
#define MAX_BUFFER_LENGTH_DEFAULT MAX_HOST_COUNT_DEFAULT*sizeof(FBSDAddress)

enum class EWatMessageType {Error = 0, HostPresentation = 1, ClientPresentation = 2, HostCount = 2, ClientConnectionRequest = 3, HostAddress = 4, SessionRequest = 5, SessionSize = 6, JoinRequest = 7};

struct FBSDAddress
{
	uint8 FamilyBytes[sizeof(int16)];
	uint8 PortBytes[sizeof(int16)];
	uint8 IpBytes[sizeof(int32)];
	uint8 Zeros[8];
};

struct FMessage
{
	EWatMessageType MessageType;
	FBSDAddress BSDAddr;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionStarted, class FInternetAddr*);
typedef FOnSessionStarted::FDelegate FOnSessionStartedDelegate;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnValidSessionRequest, class FOnlineSearchSettings*, class FInternetAddr*);
typedef FOnValidSessionRequest::FDelegate FOnValidSessionRequestDelegate;

DECLARE_MULTICAST_DELEGATE(FOnJoinRequest);
typedef FOnJoinRequest::FDelegate FOnJoinRequestDelegate;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnValidResponsePacket, uint8*, int32);
typedef FOnValidResponsePacket::FDelegate FOnValidResponsePacketDelegate;

DECLARE_MULTICAST_DELEGATE(FOnSearchingTimeout);
typedef FOnSearchingTimeout::FDelegate FOnSearchingTimeoutDelegate;

class FWLANManager final
{
private:
	class FSocket* Socket;
	TSharedPtr<class FInternetAddr> RelationServerAddress;
	TArray<TSharedPtr<class FInternetAddr> > HostsAddresses;
	bool bIsHost;
	int MaxHostCount;
	int MaxBufferLength;
	int ExpectedHostsCount;
	class FOnlineSessionSearch* ClientSessionSearch;
    int ExpectedSessionSize;
	int Port;
	int ExpectedSessionSettingsSize;
	
public:
	
	FWLANManager() : Socket(nullptr), RelationServerAddress(nullptr), bIsHost(false), MaxHostCount(MAX_HOST_COUNT_DEFAULT), MaxBufferLength(MAX_BUFFER_LENGTH_DEFAULT), ExpectedHostsCount(0), ClientSessionSearch(nullptr), ExpectedSessionSize(0), Port(0), ExpectedSessionSettingsSize(0){}

	virtual ~FWLANManager();

	//TODO relation server addr & port parameters
	bool Init();

	int GetPort() const;

	bool SendPresentationMessage();

	bool SendSession(class FNamedOnlineSession* HostSession, FInternetAddr* ClientAddress);

    bool SendJoinRequest(FInternetAddr* HostAddress);

	TSharedPtr<class FInternetAddr> ReadAddressFromBSDAddress(FBSDAddress Address) const;
	
	TSharedPtr<class FInternetAddr> ReadAddressFromMessage(FMessage Message) const;
	
	void WriteAddressIntoMessage(FMessage& Message, TSharedPtr<class FInternetAddr> Address) const;

	/**
	 * Creates the LAN beacon for queries/advertising servers
	 *
	 * @param SessionStartedDelegate delegate to fire when session is successfuly started (relation server sent back our public ip and used port)
	 * @param SessionRequestDelegate delegate to fire when a client session query is received
	 * @param JoinRequestDelegate delegate to fire when a client join query is received
	 */
	bool Host(FOnSessionStartedDelegate& SessionStartedDelegate, FOnValidSessionRequestDelegate& SessionRequestDelegate, FOnJoinRequestDelegate& JoinRequestDelegate);

	/**
	 * Creates the LAN beacon for queries/advertising servers
	 *
	 * @param SearchSettings the settings searched session must correspond to
	 * @param TimeoutDelegate delegate to fire if we exceed maximum search time
	 *
	 * @return true if search was started successfully, false otherwise
	 */
	bool Search(const TSharedRef<FOnlineSessionSearch>& SearchSettings, FOnSearchingTimeoutDelegate& TimeoutDelegate);

	//TODO timeout
	bool SendNatPunchThroughPacket(TSharedPtr<class FInternetAddr> PeerAddress, uint8* PunchPacket);

	/**
	 * Send the relation server that we're ne longer searching for clients
	 */
	void StopWLANProcess();

	static void AppendSearchSettingsToPacket(class FNboSerializeToBufferWat& Packet, FOnlineKeyValuePairs<FName, class FOnlineSessionSearchParam>& SearchParams);
	
	void AppendSessionToPacket(class FNboSerializeToBufferWat& Packet, class FOnlineSession* Session) const;

	static void AppendSessionSettingsToPacket(class FNboSerializeToBufferWat& Packet, class FOnlineSessionSettings* SessionSettings);

	static void ReadSearchSettingsFromPacket(class FNboSerializeFromBufferWat& Packet, FOnlineSearchSettings& SearchSettings);

	void ReadSessionFromPacket(class FNboSerializeFromBufferWat& Packet, FOnlineSession* Session);

	static void ReadSettingsFromPacket(class FNboSerializeFromBufferWat& Packet, FOnlineSessionSettings& SessionSettings);

	void Tick(float DeltaTime);

	DEFINE_ONLINE_DELEGATE(OnJoinRequest);
	DEFINE_ONLINE_DELEGATE_ONE_PARAM(OnSessionStarted, class FInternetAddr*);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnValidSessionRequest, class FOnlineSearchSettings*, class FInternetAddr*);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnValidResponsePacket, uint8*, int32);
	DEFINE_ONLINE_DELEGATE(OnSearchingTimeout);
};	