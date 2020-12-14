#include "OnlineSessionWaT.h"

#include "NboSerializerWat.h"
#include "Misc/Guid.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInfoWat.h"
#include "OnlineSubsystemUtils.h"
#include "IpNetDriver.h"

//==========================FOnlineSessionInfoWat===============================//

void FOnlineSessionInfoWat::Init(TSharedPtr<FInternetAddr> Addr)
{
	HostAddr = Addr->Clone();

	FGuid OwnerGuid;
	FPlatformMisc::CreateGuid(OwnerGuid);
	SessionId = FUniqueNetIdWat(OwnerGuid.ToString());
}

//==========================FOnlineSessionWat==================================//

int FOnlineSessionWat::GetPort() const
{
  return WLANSessionManager.GetPort();
}

//Session complete delegate is fired with "true" only after relation server has returned host's own public IP
bool FOnlineSessionWat::CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{	
	// Check for an existing session
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (!Session )
	{
		// Create a new session and deep copy the game settings
		Session = AddNamedSession(SessionName, NewSessionSettings);
		check(Session);		  
		Session->SessionState = EOnlineSessionState::Creating;
		Session->NumOpenPrivateConnections = NewSessionSettings.NumPrivateConnections;
		Session->NumOpenPublicConnections = NewSessionSettings.NumPublicConnections;	// always start with full public connections, local player will register later

		Session->HostingPlayerNum = HostingPlayerNum;

		check(WatSubsystem);
		const IOnlineIdentityPtr Identity = WatSubsystem->GetIdentityInterface();
		if (Identity.IsValid())
		{
			Session->OwningUserId = Identity->GetUniquePlayerId(HostingPlayerNum);
			Session->OwningUserName = Identity->GetPlayerNickname(HostingPlayerNum);
		}

		// if did not get a valid one, use just something
		if (!Session->OwningUserId.IsValid())
		{
			Session->OwningUserId = MakeShareable(new FUniqueNetIdWat(FString::Printf(TEXT("%d"), HostingPlayerNum)));
			Session->OwningUserName = FString(TEXT("WatUser"));
		}
		
		// Unique identifier of this build for compatibility
		Session->SessionSettings.BuildUniqueId = GetBuildUniqueId();

		FOnSessionStartedDelegate SessionStartedCompleteDelegate = FOnSessionStartedDelegate::CreateRaw(this, &FOnlineSessionWat::OnSessionStartDelegate);
		FOnValidSessionRequestDelegate SessionRequestDelegate = FOnValidSessionRequestDelegate::CreateRaw(this, &FOnlineSessionWat::OnValidSessionRequestDelegate);
		FOnJoinRequestDelegate JoinRequestDelegate = FOnJoinRequestDelegate::CreateRaw(this, &FOnlineSessionWat::OnJoinSessionDelegate);
		if(WLANSessionManager.Host(SessionStartedCompleteDelegate, SessionRequestDelegate, JoinRequestDelegate))
		{
			return true;
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Cannot create session '%s': session already exists."), *SessionName.ToString());
	}
	TriggerOnCreateSessionCompleteDelegates(SessionName, false);
	
	return false;
}

bool FOnlineSessionWat::CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
	return CreateSession(0, SessionName, NewSessionSettings);
}

//On session started delegate is fired with "true" only after client has requested connection
bool FOnlineSessionWat::StartSession(FName SessionName)
{
	// Grab the session information by name
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		// Can't start a match multiple times
		if (Session->SessionState == EOnlineSessionState::Pending || Session->SessionState == EOnlineSessionState::Ended)
		{
			Session->SessionState = EOnlineSessionState::InProgress;
			return true;
		}
		UE_LOG_ONLINE_SESSION(Warning,	TEXT("Can't start an online session (%s) in state %s"), *SessionName.ToString(), EOnlineSessionState::ToString(Session->SessionState));		
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't start an online game for session (%s) that hasn't been created"), *SessionName.ToString());
	}
	TriggerOnStartSessionCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionWat::UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData)
{
	bool bSuccess = false;
	
	// Grab the session information by name
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		Session->SessionSettings = UpdatedSessionSettings;
		
		bSuccess = true;
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("No session to update"));
	}

	
	TriggerOnUpdateSessionCompleteDelegates(SessionName, bSuccess);
	return bSuccess;
}

bool FOnlineSessionWat::EndSession(FName SessionName)
{
	bool bSuccess = false;
	
	// Grab the session information by name
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		// Can't end a match that isn't in progress
		if (Session->SessionState == EOnlineSessionState::InProgress)
		{
			Session->SessionState = EOnlineSessionState::Ended;
			bSuccess = true;
		}
		else
		{
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't end session (%s) in state %s"), *SessionName.ToString(), EOnlineSessionState::ToString(Session->SessionState));
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't end an online game for session (%s) that hasn't been created"), *SessionName.ToString());
	}
	
	TriggerOnEndSessionCompleteDelegates(SessionName, bSuccess);
	return bSuccess;
}

bool FOnlineSessionWat::DestroySession(FName SessionName, const FOnDestroySessionCompleteDelegate& CompletionDelegate)
{
	bool bSuccess = false;
	
	// Find the session in question
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		// The session info is no longer needed
		RemoveNamedSession(Session->SessionName);
		bSuccess = true;
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't destroy a null online session (%s)"), *SessionName.ToString());
	}

	// ReSharper disable once CppExpressionWithoutSideEffects
	CompletionDelegate.ExecuteIfBound(SessionName, bSuccess);
	TriggerOnDestroySessionCompleteDelegates(SessionName, bSuccess);
	return bSuccess;
}

bool FOnlineSessionWat::IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId)
{
	return IsPlayerInSessionImpl(this, SessionName, UniqueId);
}

bool FOnlineSessionWat::StartMatchmaking(const TArray< TSharedRef<const FUniqueNetId> >& LocalPlayers, FName SessionName, const FOnlineSessionSettings& NewSessionSettings, TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	UE_LOG_ONLINE_SESSION(Warning, TEXT("StartMatchmaking is not supported on this platform. Use FindSessions or FindSessionById."));
	TriggerOnMatchmakingCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionWat::CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName)
{
	UE_LOG_ONLINE_SESSION(Warning, TEXT("CancelMatchmaking is not supported on this platform. Use CancelFindSessions."));
	TriggerOnCancelMatchmakingCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionWat::CancelMatchmaking(const FUniqueNetId& SearchingPlayerId, FName SessionName)
{
	UE_LOG_ONLINE_SESSION(Warning, TEXT("CancelMatchmaking is not supported on this platform. Use CancelFindSessions."));
	TriggerOnCancelMatchmakingCompleteDelegates(SessionName, false);
	return false;
}

bool FOnlineSessionWat::FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	// Don't start another search while one is in progress
	if (!CurrentSessionSearch.IsValid() && SearchSettings->SearchState != EOnlineAsyncTaskState::InProgress)
	{
		// Free up previous results
		SearchSettings->SearchResults.Empty();

		// Copy the search pointer so we can keep it around
		CurrentSessionSearch = SearchSettings;

		FOnSearchingTimeoutDelegate TimeoutDelegate = FOnSearchingTimeoutDelegate::CreateRaw(this, &FOnlineSessionWat::OnSearchTimeoutDelegate);
		if(WLANSessionManager.Search(SearchSettings, TimeoutDelegate))
		{
			SearchSettings->SearchState = EOnlineAsyncTaskState::InProgress;
			return true;
		}	
	}

	UE_LOG_ONLINE_SESSION(Warning, TEXT("Ignoring game search request while one is pending"));
	return false;
}

bool FOnlineSessionWat::FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	// This function doesn't use the SearchingPlayerNum parameter, so passing in anything is fine.
	return FindSessions(0, SearchSettings);
}

bool FOnlineSessionWat::FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegates)
{
	const FOnlineSessionSearchResult EmptyResult;
	// ReSharper disable once CppExpressionWithoutSideEffects
	CompletionDelegates.ExecuteIfBound(0, false, EmptyResult);
	return true;
}

bool FOnlineSessionWat::CancelFindSessions()
{
	if (CurrentSessionSearch.IsValid() && CurrentSessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		//TODO act on WLANManager

		CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Failed;
		CurrentSessionSearch = nullptr;
		TriggerOnCancelFindSessionsCompleteDelegates(true);
		return true;
	}

	UE_LOG_ONLINE_SESSION(Warning, TEXT("Can't cancel a search that isn't in progress"));
	return false;
}

uint32 FOnlineSessionWat::JoinWLANSession(int32 PlayerNum, FNamedOnlineSession* Session, const FOnlineSession* SearchSession)
{
	check(Session != nullptr);


	if (Session->SessionInfo.IsValid() && SearchSession != nullptr && SearchSession->SessionInfo.IsValid())
	{
		// Copy the session info over
		const FOnlineSessionInfoWat* SearchSessionInfo = static_cast<const FOnlineSessionInfoWat*>(SearchSession->SessionInfo.Get());
		FOnlineSessionInfoWat* SessionInfo = static_cast<FOnlineSessionInfoWat*>(Session->SessionInfo.Get());
		SessionInfo->SessionId = SearchSessionInfo->SessionId;
		SessionInfo->HostAddr = SearchSessionInfo->HostAddr->Clone();

		Session->SessionState = EOnlineSessionState::Pending;
		WLANSessionManager.SendJoinRequest(SearchSessionInfo->HostAddr.Get());
	    UE_LOG(LogOnline, Warning, TEXT("%s"), *SessionInfo->HostAddr->ToString(true));
		WLANSessionManager.StopWLANProcess();
		return true;
	}

	return false;
}

bool FOnlineSessionWat::JoinSession(int32 PlayerNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession)
{
	bool bSuccess = false;
	
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	// Don't join a session if already in one or hosting one
	if (Session == nullptr)
	{
		// Create a named session from the search result data
		Session = AddNamedSession(SessionName, DesiredSession.Session);
		Session->HostingPlayerNum = PlayerNum;

		// Create Session Infos
		FOnlineSessionInfoWat* NewSessionInfo = new FOnlineSessionInfoWat();
		Session->SessionInfo = MakeShareable(NewSessionInfo);

		if(JoinWLANSession(PlayerNum, Session, &DesiredSession.Session))
		{
			Session->SessionSettings.bShouldAdvertise = false;
			bSuccess = true;
		}
		else
		{
			//Clean up the session info so we don't get into a confused state
			RemoveNamedSession(SessionName);
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Session (%s) already exists, can't join twice"), *SessionName.ToString());
	}

	TriggerOnJoinSessionCompleteDelegates(SessionName, bSuccess ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
	return bSuccess;
}

bool FOnlineSessionWat::JoinSession(const FUniqueNetId& PlayerId, FName SessionName, const FOnlineSessionSearchResult& DesiredSession)
{
	// Assuming player 0 should be OK here
	return JoinSession(0, SessionName, DesiredSession);
}

bool FOnlineSessionWat::FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	const TArray<FOnlineSessionSearchResult> EmptySearchResult;
	TriggerOnFindFriendSessionCompleteDelegates(LocalUserNum, false, EmptySearchResult);
	return false;
};

bool FOnlineSessionWat::FindFriendSession(const FUniqueNetId& LocalUserId, const FUniqueNetId& Friend)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	const TArray<FOnlineSessionSearchResult> EmptySearchResult;
	TriggerOnFindFriendSessionCompleteDelegates(0, false, EmptySearchResult);
	return false;
}

bool FOnlineSessionWat::FindFriendSession(const FUniqueNetId& LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& FriendList)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	const TArray<FOnlineSessionSearchResult> EmptySearchResult;
	TriggerOnFindFriendSessionCompleteDelegates(0, false, EmptySearchResult);
	return false;
}

bool FOnlineSessionWat::SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId& Friend)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	return false;
};

bool FOnlineSessionWat::SendSessionInviteToFriend(const FUniqueNetId& LocalUserId, FName SessionName, const FUniqueNetId& Friend)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	return false;
}

bool FOnlineSessionWat::SendSessionInviteToFriends(int32 LocalUserNum, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	return false;
};

bool FOnlineSessionWat::SendSessionInviteToFriends(const FUniqueNetId& LocalUserId, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends)
{
	// this function has to exist due to interface definition, but it does not have a meaningful implementation in Wat subsystem
	return false;
}

bool FOnlineSessionWat::PingSearchResults(const FOnlineSessionSearchResult& SearchResult)
{
	return false;
}

/** Get a resolved connection string from a session info */
static void GetConnectStringFromSessionInfo(TSharedPtr<FOnlineSessionInfoWat>& SessionInfo, FString& ConnectInfo)
{
	if (SessionInfo.IsValid() && SessionInfo->HostAddr->IsValid())
	{
		ConnectInfo = SessionInfo->HostAddr->ToString(true);
	}
}

bool FOnlineSessionWat::GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType)
{
	bool bSuccess = true;
	// Find the session
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session != nullptr)
	{
		TSharedPtr<FOnlineSessionInfoWat> SessionInfo = StaticCastSharedPtr<FOnlineSessionInfoWat>(Session->SessionInfo);

		GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo);


		if (ConnectInfo.IsEmpty())
		{
			bSuccess = false;
			UE_LOG_ONLINE_SESSION(Warning, TEXT("Invalid session info for session %s in GetResolvedConnectString()"), *SessionName.ToString());
		}
	}
	else
	{
		bSuccess = false;
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Unknown session name (%s) specified to GetResolvedConnectString()"), *SessionName.ToString());
	}

	return bSuccess;
}

bool FOnlineSessionWat::GetResolvedConnectString(const FOnlineSessionSearchResult& SearchResult, FName PortType, FString& ConnectInfo)
{
	if (SearchResult.Session.SessionInfo.IsValid())
	{
		TSharedPtr<FOnlineSessionInfoWat> SessionInfo = StaticCastSharedPtr<FOnlineSessionInfoWat>(SearchResult.Session.SessionInfo);
		GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo);
	}
	
	if (ConnectInfo.IsEmpty())
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("Invalid session info in search result to GetResolvedConnectString()"));
		return false;
	}

	return true;
}

FOnlineSessionSettings* FOnlineSessionWat::GetSessionSettings(FName SessionName) 
{
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		return &Session->SessionSettings;
	}
	return nullptr;
}

bool FOnlineSessionWat::RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited)
{
	TArray< TSharedRef<const FUniqueNetId> > Players;
	Players.Add(MakeShareable(new FUniqueNetIdWat(PlayerId)));
	return RegisterPlayers(SessionName, Players, bWasInvited);
}

bool FOnlineSessionWat::RegisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players, bool bWasInvited)
{
	bool bSuccess = false;
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		bSuccess = true;
		for (int32 PlayerIdx=0; PlayerIdx<Players.Num(); PlayerIdx++)
		{
			const TSharedRef<const FUniqueNetId>& PlayerId = Players[PlayerIdx];

			const FUniqueNetIdMatcher PlayerMatch(*PlayerId);
			if (Session->RegisteredPlayers.IndexOfByPredicate(PlayerMatch) == INDEX_NONE)
			{
				Session->RegisteredPlayers.Add(PlayerId);

				// update number of open connections
				if (Session->NumOpenPublicConnections > 0)
				{
					Session->NumOpenPublicConnections--;
				}
				else if (Session->NumOpenPrivateConnections > 0)
				{
					Session->NumOpenPrivateConnections--;
				}
				
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Log, TEXT("Player %s already registered in session %s"), *PlayerId->ToDebugString(), *SessionName.ToString());
			}			
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("No game present to join for session (%s)"), *SessionName.ToString());
	}

	TriggerOnRegisterPlayersCompleteDelegates(SessionName, Players, bSuccess);
	return bSuccess;
}

bool FOnlineSessionWat::UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId)
{
	TArray< TSharedRef<const FUniqueNetId> > Players;
	Players.Add(MakeShareable(new FUniqueNetIdWat(PlayerId)));
	return UnregisterPlayers(SessionName, Players);
}

bool FOnlineSessionWat::UnregisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players)
{
	bool bSuccess = true;

	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	if (Session)
	{
		for (int32 PlayerIdx=0; PlayerIdx < Players.Num(); PlayerIdx++)
		{
			const TSharedRef<const FUniqueNetId>& PlayerId = Players[PlayerIdx];

			const FUniqueNetIdMatcher PlayerMatch(*PlayerId);
			const int32 RegistrantIndex = Session->RegisteredPlayers.IndexOfByPredicate(PlayerMatch);
			if (RegistrantIndex != INDEX_NONE)
			{
				Session->RegisteredPlayers.RemoveAtSwap(RegistrantIndex);

				// update number of open connections
				if (Session->NumOpenPublicConnections < Session->SessionSettings.NumPublicConnections)
				{
					Session->NumOpenPublicConnections++;
				}
				else if (Session->NumOpenPrivateConnections < Session->SessionSettings.NumPrivateConnections)
				{
					Session->NumOpenPrivateConnections++;
				}
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Warning, TEXT("Player %s is not part of session (%s)"), *PlayerId->ToDebugString(), *SessionName.ToString());
			}
		}
	}
	else
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("No game present to leave for session (%s)"), *SessionName.ToString());
		bSuccess = false;
	}

	TriggerOnUnregisterPlayersCompleteDelegates(SessionName, Players, bSuccess);
	return bSuccess;
}

void FOnlineSessionWat::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Session_Interface);
	WLANSessionManager.Tick(DeltaTime);
}

void FOnlineSessionWat::OnValidSessionRequestDelegate(FOnlineSearchSettings* OnlineSearchSettings, FInternetAddr* SenderAddr)
{
	//TODO check online settings
	for(auto Param : OnlineSearchSettings->SearchParams)
	{
		UE_LOG(LogOnline, Warning, TEXT("%s:%s"), *Param.Key.ToString(),*Param.Value.ToString())
	}
	
	WLANSessionManager.SendSession(&Sessions[0],  SenderAddr);
}

void FOnlineSessionWat::OnSessionStartDelegate(FInternetAddr* HostAddress)
{
	FOnlineSessionInfoWat* NewSessionInfo = new FOnlineSessionInfoWat();
	NewSessionInfo->Init(MakeShareable(HostAddress));
	Sessions[0].SessionInfo = MakeShareable(NewSessionInfo);
	Sessions[0].SessionState = EOnlineSessionState::Pending;
    TriggerOnCreateSessionCompleteDelegates(Sessions[0].SessionName, true);
}

void FOnlineSessionWat::OnJoinSessionDelegate()
{
	TriggerOnStartSessionCompleteDelegates(Sessions[0].SessionName, true);
}
void FOnlineSessionWat::OnSearchTimeoutDelegate()
{
	//TODO reset WLAN manager?

	if (CurrentSessionSearch.IsValid())
	{
		if (CurrentSessionSearch->SearchResults.Num() > 0)
		{
			// Allow game code to sort the servers
			CurrentSessionSearch->SortSearchResults();
		}
		CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Done;

		CurrentSessionSearch = nullptr;
	}
	// Trigger the delegate as complete
	TriggerOnFindSessionsCompleteDelegates(true);
}

int32 FOnlineSessionWat::GetNumSessions()
{
	FScopeLock ScopeLock(&SessionLock);
	return Sessions.Num();
}

void FOnlineSessionWat::DumpSessionState()
{
	FScopeLock ScopeLock(&SessionLock);

	for (int32 SessionIdx=0; SessionIdx < Sessions.Num(); SessionIdx++)
	{
		DumpNamedSession(&Sessions[SessionIdx]);
	}
}

void FOnlineSessionWat::RegisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnRegisterLocalPlayerCompleteDelegate& Delegate)
{
	// ReSharper disable once CppExpressionWithoutSideEffects
	Delegate.ExecuteIfBound(PlayerId, EOnJoinSessionCompleteResult::Success);
}

void FOnlineSessionWat::UnregisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnUnregisterLocalPlayerCompleteDelegate& Delegate)
{
	// ReSharper disable once CppExpressionWithoutSideEffects
	Delegate.ExecuteIfBound(PlayerId, true);
}

bool FOnlineSessionWat::IsHost(const FNamedOnlineSession& Session) const
{
	if (WatSubsystem->IsDedicated())
	{
		return true;
	}

	const IOnlineIdentityPtr IdentityInt = WatSubsystem->GetIdentityInterface(); 
	if (!IdentityInt.IsValid())
	{
		return false;
	}

	const TSharedPtr<const FUniqueNetId> UserId = IdentityInt->GetUniquePlayerId(Session.HostingPlayerNum);
	return (UserId.IsValid() && (*UserId == *Session.OwningUserId));
}

TSharedPtr<const FUniqueNetId> FOnlineSessionWat::CreateSessionIdFromString(const FString& SessionIdStr)
{
	TSharedPtr<const FUniqueNetId> SessionId;
	if (!SessionIdStr.IsEmpty())
	{
		SessionId = MakeShared<FUniqueNetIdWat>(SessionIdStr);
	}
	return SessionId;
}


