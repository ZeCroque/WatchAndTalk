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
  const bool bNullInit = true;
	
  if (bNullInit)
  {
	  // Create the online async task thread
	 // OnlineAsyncTaskThreadRunnable = new FOnlineAsyncTaskManagerNull(this);
	  //check(OnlineAsyncTaskThreadRunnable);
	  //OnlineAsyncTaskThread = FRunnableThread::Create(OnlineAsyncTaskThreadRunnable, *FString::Printf(TEXT("OnlineAsyncTaskThreadNull %s(%d)"), *InstanceName.ToString(), TaskCounter.Increment()), 128 * 1024, TPri_Normal);
	  //check(OnlineAsyncTaskThread);
	  //UE_LOG_ONLINE(Verbose, TEXT("Created thread (ID:%d)."), OnlineAsyncTaskThread->GetThreadID());

	  SessionInterface = MakeShareable(new FOnlineSessionWat(this));
	  //LeaderboardsInterface = MakeShareable(new FOnlineLeaderboardsNull(this));
	  IdentityInterface = MakeShareable(new FOnlineIdentityWat(this));
	  /*AchievementsInterface = MakeShareable(new FOnlineAchievementsNull(this));
	  VoiceInterface = MakeShareable(new FOnlineVoiceImpl(this));
	  StoreV2Interface = MakeShareable(new FOnlineStoreV2Null(*this));
	  PurchaseInterface = MakeShareable(new FOnlinePurchaseNull(*this));*/
  }
  else
  {
      Shutdown();
  }

  return bNullInit;
}

bool FOnlineSubsystemWaT::Shutdown()
{
  UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemNull::Shutdown()"));

  FOnlineSubsystemImpl::Shutdown();

  /*if (OnlineAsyncTaskThread)
  {
	  // Destroy the online async task thread
	  delete OnlineAsyncTaskThread;
	  OnlineAsyncTaskThread = nullptr;
  }*/

  /*if (OnlineAsyncTaskThreadRunnable)
  {
	  delete OnlineAsyncTaskThreadRunnable;
	  OnlineAsyncTaskThreadRunnable = nullptr;
  }*/

  /*if (VoiceInterface.IsValid() && bVoiceInterfaceInitialized)
  {
	  VoiceInterface->Shutdown();
  }*/

  #define DESTRUCT_INTERFACE(Interface) \
  if (Interface.IsValid()) \
  { \
	  ensure(Interface.IsUnique()); \
	  Interface = nullptr; \
  }

  // Destruct the interfaces
  /*DESTRUCT_INTERFACE(PurchaseInterface);
  DESTRUCT_INTERFACE(StoreV2Interface);
  DESTRUCT_INTERFACE(VoiceInterface);
  DESTRUCT_INTERFACE(AchievementsInterface);*/
  DESTRUCT_INTERFACE(IdentityInterface);
  //DESTRUCT_INTERFACE(LeaderboardsInterface);
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
