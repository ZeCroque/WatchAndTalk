#include "OnlineSubsystemWat.h"
#include "Containers/UnrealString.h"
#include "Internationalization/Text.h"
#include "OnlineSessionWat.h"
#include "OnlineIdentityWat.h"

IOnlineSessionPtr FOnlineSubsystemWat::GetSessionInterface() const
{
	return SessionInterface;
}

IOnlineIdentityPtr FOnlineSubsystemWat::GetIdentityInterface() const
{
	return IdentityInterface;
}

IOnlineFriendsPtr FOnlineSubsystemWat::GetFriendsInterface() const
{
	return nullptr;
}

IOnlineStorePtr FOnlineSubsystemWat::GetStoreInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemWat::GetTurnBasedInterface() const
{
	return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemWat::GetTournamentInterface() const
{
	return nullptr;
}

bool FOnlineSubsystemWat::Init()
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

bool FOnlineSubsystemWat::Shutdown()
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

FString FOnlineSubsystemWat::GetAppId() const
{
  return FString();
}

bool FOnlineSubsystemWat::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
  if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
  {
    return true;
  }
  return false;
}

FText FOnlineSubsystemWat::GetOnlineServiceName() const
{
	return FText::FromString(TEXT("WatOSS"));
}
int FOnlineSubsystemWat::GetPort() const
{
  return SessionInterface->GetPort();
}

bool FOnlineSubsystemWat::Tick(float DeltaTime)
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
