#include "OnlineSubsystemInterface.h"
#include "Containers/UnrealString.h"
#include "Internationalization/Text.h"
#include "OnlineSessionWaT.h"
#include "OnlineIdentityWat.h"

IOnlineSessionPtr FOnlineSubsystemWaT::GetSessionInterface() const
{
	return SessionInterface;
}

IOnlineIdentityPtr FOnlineSubsystemWaT::GetIdentityInterface() const
{
	return IdentityInterface;
}

IOnlineFriendsPtr FOnlineSubsystemWaT::GetFriendsInterface() const
{
	return nullptr;
}

IOnlineStorePtr FOnlineSubsystemWaT::GetStoreInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemWaT::GetTurnBasedInterface() const
{
	return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemWaT::GetTournamentInterface() const
{
	return nullptr;
}

bool FOnlineSubsystemWaT::Init()
{
	
  if (!SessionInterface && !IdentityInterface)
  {
	  SessionInterface = MakeShareable(new FOnlineSessionWat(this));
	  IdentityInterface = MakeShareable(new FOnlineIdentityWat(this));
  	  return true;
  }

  Shutdown();
  return false;
}

bool FOnlineSubsystemWaT::Shutdown()
{
  UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemNull::Shutdown()"));

  FOnlineSubsystemImpl::Shutdown();

  #define DESTRUCT_INTERFACE(Interface) \
  if (Interface.IsValid()) \
  { \
	  ensure(Interface.IsUnique()); \
	  Interface = nullptr; \
  }

  // Destruct the interfaces
  DESTRUCT_INTERFACE(IdentityInterface);
  DESTRUCT_INTERFACE(SessionInterface);

  #undef DESTRUCT_INTERFACE

  return true;
}

FString FOnlineSubsystemWaT::GetAppId() const
{
  return FString();
}

bool FOnlineSubsystemWaT::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
  if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
  {
    return true;
  }
  return false;
}

FText FOnlineSubsystemWaT::GetOnlineServiceName() const
{
	return FText::FromString(TEXT("WatOSS"));
}
int FOnlineSubsystemWaT::GetPort() const
{
  return SessionInterface->GetPort();
}

bool FOnlineSubsystemWaT::Tick(float DeltaTime)
{
  if (!FOnlineSubsystemImpl::Tick(DeltaTime))
  {
    return false;
  }

  if (SessionInterface.IsValid())
  {
    SessionInterface->Tick(DeltaTime);
  }

  return true;
}
