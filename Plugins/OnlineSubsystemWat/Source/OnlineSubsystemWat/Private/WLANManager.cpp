#include "WLANManager.h"
#include "OnlineSubsystemUtils.h"

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
		AddOnValidQueryPacketDelegate_Handle(QueryDelegate);
		//TODO change host status
		return true;
	}
	return false;
}

bool FWLANManager::Search(FOnValidResponsePacketDelegate& ResponseDelegate, FOnSearchingTimeoutDelegate& TimeoutDelegate)
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

	//TODO check errors
	return true;
}

void FWLANManager::StopWLANProcess()
{
	if (RelationServerAddr)
	{
		delete RelationServerAddr;
		RelationServerAddr = nullptr;
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
				case MessageType::TestMessage:
					UE_LOG(LogOnline, Warning, TEXT("Received message of type:%d\nDone."), static_cast<int>(Message->messageType));
					break;
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
				case MessageType::TestMessage:
					UE_LOG(LogOnline, Warning, TEXT("Received message of type:%d"), static_cast<int>(Message->messageType));
					sendto(Socket, reinterpret_cast<char*>(Message), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(&SenderAddr), sizeof(SenderAddr));
					UE_LOG(LogOnline, Warning, TEXT("Test answer sent.\nDone."));
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
			//DEBUG
			if (bIsHost)
			{
				FMessage Message = { MessageType::TestMessage, {0, 0, {{{0,0,0,0}}}, {0}} };
				sendto(Socket, reinterpret_cast<char*>(&Message), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(&SenderAddr), sizeof(SenderAddr));
				UE_LOG(LogOnline, Warning, TEXT("Sent test message to client."));
			}
			UE_LOG(LogOnline, Warning, TEXT("NAT punch through successful."));
		}
		//Handle Session package
		//TriggerOnValidResponsePacketDelegates();
		//TriggerOnValidResponsePacketDelegates(&PacketData[LAN_BEACON_PACKET_HEADER_SIZE], NumRead - LAN_BEACON_PACKET_HEADER_SIZE);
		//Host addresses
		else if (ReceivedSize == ExpectedHostsCount * sizeof(SOCKADDR_IN))
		{
			UE_LOG(LogOnline, Warning, TEXT("Hosts addresses received. Hosts list:"));
			HostsAddresses = new SOCKADDR_IN[ExpectedHostsCount];
			for (int i = 0; i < ExpectedHostsCount; i++)
			{
				//TODO connect to everyone
				HostsAddresses[i] = *reinterpret_cast<SOCKADDR_IN*>(ReceptionBuffer + i * sizeof(SOCKADDR_IN));
				char Ip[16];
				inet_ntop(AF_INET, reinterpret_cast<void*>(&HostsAddresses[i].sin_addr), Ip, 16);
			    auto IpString = FString(Ip);
				UE_LOG(LogOnline, Warning, TEXT("%s:%d"), *IpString, ntohs(HostsAddresses[i].sin_port));
			}

			FMessage Answer = { MessageType::ClientConnectionRequest, HostsAddresses[0] };
			sendto(Socket, reinterpret_cast<char*>(&Answer), sizeof(FMessage), 0, reinterpret_cast<SOCKADDR*>(RelationServerAddr), sizeof(*RelationServerAddr));
			UE_LOG(LogOnline, Warning, TEXT("Connection request sent to server."));

			SendNatPunchThroughPacket(HostsAddresses[0], "one");
			UE_LOG(LogOnline, Warning, TEXT("NAT Punching package sent to host..."));
			ExpectedHostsCount = -1;
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
//

/*enum class MessageType {Error = 0, HostPresentation = 1, ClientPresentation = 2, HostCount = 2, ClientConnectionRequest = 3, TestMessage = 4};

struct Message
{
MessageType messageType;
SOCKADDR_IN adr;
};

enum class ProcessMode {None = 0, Host = 1, Client = 2};

void clientRoutine(SOCKET serviceSocket, SOCKADDR_IN& sin);
void hostRoutine(SOCKET serviceSocket, SOCKADDR_IN& sin);
void natPunchRoutine(SOCKET serviceSocket, SOCKADDR_IN& sin);

int main(const int argc, char** argv)
{
if(argc == 2)
{
const ProcessMode mode = !strcmp(argv[1],"-c")? ProcessMode::Client:!strcmp(argv[1],"-h")? ProcessMode::Host: ProcessMode::None;

if(mode != ProcessMode::None)
{
WSADATA wsaData;
if(!WSAStartup(MAKEWORD(2,0), &wsaData))
{
SOCKADDR_IN sin;
InetPton(AF_INET, L"193.250.68.210", &sin.sin_addr.s_addr);
sin.sin_family = AF_INET;
sin.sin_port = htons(7777);
const SOCKET serviceSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
UE_LOG(LogOnline, Warning, TEXT("Socket created. Done."));
// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
// ReSharper disable once CppIncompleteSwitchStatement
switch(mode)  // NOLINT(clang-diagnostic-switch)
{
case ProcessMode::Client:
clientRoutine(serviceSocket, sin);
break;
case ProcessMode::Host:
hostRoutine(serviceSocket, sin);
break;
}

closesocket(serviceSocket);
WSACleanup();
UE_LOG(LogOnline, Warning, TEXT("Socket closed. Done."));
return 0;
}
UE_LOG(LogOnline, Warning, TEXT("Error:WSA couldn't start"));
return 0;
}
}

UE_LOG(LogOnline, Warning, TEXT("You must specify '-c' to be the client or 'h' to be the host"));
return 0;
}

void clientRoutine(const SOCKET serviceSocket, SOCKADDR_IN& sin)
{
Message message {MessageType::Error, {0, 0, {{{0,0,0,0}}}, {0}}};
message.messageType = MessageType::ClientPresentation;
sendto(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<SOCKADDR*>(&sin), sizeof(sin));
UE_LOG(LogOnline, Warning, TEXT("Presentation message sent. Waiting for host count message..."));

memset(&message,0,sizeof(Message));
recv(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0);
if(message.messageType != MessageType::HostCount)
{
UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message."));
exit(-1);
}
const int hostCount = *reinterpret_cast<int*>(&message.adr);
UE_LOG(LogOnline, Warning, TEXT("Host count received: %d\nWaiting for host addresses..."), hostCount);

auto* hostsAdr =  new SOCKADDR_IN[hostCount];
recv(serviceSocket, reinterpret_cast<char*>(hostsAdr), static_cast<int>(sizeof(SOCKADDR_IN))*hostCount, 0);
UE_LOG(LogOnline, Warning, TEXT("Hosts addresses received. Hosts list:"));
for(int i=0; i<hostCount; ++i)
{
char buffer[16];
inet_ntop(AF_INET, reinterpret_cast<void*>(&hostsAdr[i].sin_addr), buffer, 16);
UE_LOG(LogOnline, Warning, TEXT("%s:%d"), buffer, ntohs(hostsAdr[i].sin_port));
}

message = {MessageType::ClientConnectionRequest, hostsAdr[0]};
sendto(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<SOCKADDR*>(&sin), sizeof(sin));
UE_LOG(LogOnline, Warning, TEXT("Connection request sent to host."));

SOCKADDR_IN& hostAddress = hostsAdr[0];
natPunchRoutine(serviceSocket, hostAddress);

memset(&message, 0, sizeof(Message));
UE_LOG(LogOnline, Warning, TEXT("Waiting for host test message..."));
recv(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message),0);
UE_LOG(LogOnline, Warning, TEXT("Received message of type:%d"), static_cast<int>(message.messageType));

sendto(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message),0,reinterpret_cast<SOCKADDR*>(&hostAddress), sizeof(hostAddress));
UE_LOG(LogOnline, Warning, TEXT("Test answer sent."));
}

void hostRoutine(const SOCKET serviceSocket, SOCKADDR_IN& sin)
{
Message message = {MessageType::HostPresentation, {0, 0, {{{0,0,0,0}}}, {0}}};
sendto(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<SOCKADDR*>(&sin), sizeof(sin));
UE_LOG(LogOnline, Warning, TEXT("Presentation message sent. Waiting for connection request..."));

memset(&message,0,sizeof(Message));
recv(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0);
if(message.messageType != MessageType::ClientConnectionRequest)
{
UE_LOG(LogOnline, Warning, TEXT("Error: Unexpected message. Size:"));
exit(-1);
}
UE_LOG(LogOnline, Warning, TEXT("Connection request received."));

SOCKADDR_IN clientAddress = message.adr;
natPunchRoutine(serviceSocket, clientAddress);

message = {MessageType::TestMessage, {0, 0, {{{0,0,0,0}}}, {0}}};
sendto(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0, reinterpret_cast<SOCKADDR*>(&clientAddress), sizeof(clientAddress));
UE_LOG(LogOnline, Warning, TEXT("Sent test message to client. Waiting for answer..."));

memset(&message,0,sizeof(Message));
recv(serviceSocket, reinterpret_cast<char*>(&message), sizeof(Message), 0);
UE_LOG(LogOnline, Warning, TEXT("Received message of type:%d"), static_cast<int>(message.messageType));
}

void natPunchRoutine(SOCKET serviceSocket, SOCKADDR_IN& sin)
{
char buffer[16];
inet_ntop(AF_INET, reinterpret_cast<void*>(&sin.sin_addr), buffer, 16);
UE_LOG(LogOnline, Warning, TEXT("Nat punching client %s:%d"),buffer,ntohs(sin.sin_port));

std::string punchMessage = "one";
char inputPunchBuffer[4] = {'\0'};
sendto(serviceSocket, punchMessage.c_str(), static_cast<int>(punchMessage.size())+1, 0, reinterpret_cast<SOCKADDR*>(&sin), sizeof(sin));
UE_LOG(LogOnline, Warning, TEXT("Punch packet sent. Trying to receive peer punch packet..."));
recv(serviceSocket, inputPunchBuffer, sizeof(char)*4,0);
if(!strcmp(inputPunchBuffer, punchMessage.c_str()))
{
UE_LOG(LogOnline, Warning, TEXT("Peer punched our NAT first. Trying to punch his again..."));
punchMessage = "two";
sendto(serviceSocket, punchMessage.c_str(), static_cast<int>(punchMessage.size())+1, 0, reinterpret_cast<SOCKADDR*>(&sin), sizeof(sin));
}

UE_LOG(LogOnline, Warning, TEXT("NAT punch through successful."));
}*/
