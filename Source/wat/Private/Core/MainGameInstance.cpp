// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Core/NetworkSettingsWat.h"
#include "Kismet/GameplayStatics.h"

UMainGameInstance::UMainGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::OnCreateSessionComplete);
    OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::OnStartSessionComplete);
    OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMainGameInstance::OnFindSessionsComplete);
    OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::OnJoinSessionComplete);
    OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::OnDestroySessionComplete);
}

void UMainGameInstance::BeginDestroy()
{
    Super::BeginDestroy();
    DestroySessionAndLeaveGame();
}

bool UMainGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN,
                                    bool bIsPresence, int32 MaxNumPlayers)
{
    //Get subsystem
    SubsystemName = bIsLAN ? L"NULL": L"WAT";
    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(SubsystemName);

	
	
    UE_LOG(LogTemp, Warning, TEXT("UserId: %s"), *(UserId->ToDebugString()));
    UE_LOG(LogTemp, Warning, TEXT("SessionName: %s"), *(SessionName.ToString()));

    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid() && UserId.IsValid())
        {
            //Here we set all the settings for the session according to the parameters
            SessionSettings = MakeShareable(new FOnlineSessionSettings());
            SessionSettings->bIsLANMatch = bIsLAN;
            SessionSettings->bUsesPresence = bIsPresence;
            SessionSettings->NumPublicConnections = MaxNumPlayers;
            SessionSettings->NumPrivateConnections = 0;
            SessionSettings-> bAllowInvites = true;
            SessionSettings->bAllowJoinInProgress = true;
            SessionSettings-> bShouldAdvertise = true;
            SessionSettings->bAllowJoinViaPresence = true;
            SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
            SessionSettings->Set(SETTING_MAPNAME, FString("Test_Level_Design"), EOnlineDataAdvertisementType::ViaOnlineService);

            OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
            return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No OnlineSubsytem found!"));
    }
    return false;
}

void UMainGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
    SubsystemName = bIsLAN ? L"NULL": L"WAT";
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);

    if (OnlineSub)
    {

        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid() && UserId.IsValid())
        {

            SessionSearch.Reset();
            SessionSearch = MakeShareable(new FOnlineSessionSearch());
            SessionSearch->SearchState = EOnlineAsyncTaskState::NotStarted;
            SessionSearch->bIsLanQuery = bIsLAN;
            SessionSearch->MaxSearchResults = 10;
            SessionSearch->PingBucketSize = 500;

            if (bIsPresence)
            {

                SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
            }

            const TSharedRef<FOnlineSessionSearch> SearchSettingRef = SessionSearch.ToSharedRef();

            OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
            Sessions->FindSessions(*UserId, SearchSettingRef);
        }
    }
    else
    {
        OnFindSessionsComplete(false);
    }
}

bool UMainGameInstance::ConnectToSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
    const FOnlineSessionSearchResult& SearchResult)
{
    bool bSuccessful = false;

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);

    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid() && UserId.IsValid())
        {
            OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			
            bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
        }
    }
		
    return bSuccessful;
}

void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    {
        UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful);

        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
        if (OnlineSub)
        {
            IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

            if (Sessions.IsValid())
            {
                Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
                if(bWasSuccessful)
                {
                    OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
                    Sessions->StartSession(SessionName);
                }
            }
        }
    }

}

void UMainGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful);

    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (OnlineSub)
    {

        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid())
        {

            Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

            UE_LOG(LogTemp, Warning, TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num());
            OnHostListUpdate.Broadcast();
            if (SessionSearch->SearchResults.Num() > 0)
            {
                for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Session Number: %d | Sessionname: %s "), SearchIdx+1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName));
                }
            }
        }
    }
}

void UMainGameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful);

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid())
        {
            Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
        }
    }
    if (bWasSuccessful)
    {
    	const auto* NetworkSettingsWat = GetDefault<UNetworkSettingsWat>();
        UGameplayStatics::OpenLevel(GetWorld(), *NetworkSettingsWat->LobbyLevel.GetAssetName(), true, "listen");
    }
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result));

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid())
        {
            Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

            APlayerController * const PlayerController = GetFirstLocalPlayerController();

            FString TravelURL;

            if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
            {
                PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
            }
        }
    }
}

void UMainGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful);

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid())
        {
            Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
            if (bWasSuccessful)
            {
                auto* World = GetWorld();
            	if(World)
                {
            		const auto* NetworkSettingsWat = GetDefault<UNetworkSettingsWat>();				
				    UGameplayStatics::OpenLevel(World, *NetworkSettingsWat->MainMenuLevel.GetAssetName(), true);
				}
            }
        }
    }
}

bool UMainGameInstance::StartOnlineGame(FName SessionName, bool bIsLan)
{
#if WITH_EDITOR
	bIsHost = true;
#endif
	
    ULocalPlayer* const Player = GetFirstGamePlayer();
    CurrentSessionName = SessionName;
    return HostSession(Player->GetPreferredUniqueNetId()->AsShared(), CurrentSessionName, bIsLan, true, 2);
}

void UMainGameInstance::FindOnlineGames(bool bIsLAN)
{
#if WITH_EDITOR
	bIsHost = false;
#endif

    GetEngine()->OnNetworkFailure().Clear();
    GetEngine()->OnNetworkFailure().AddUObject(this, &UMainGameInstance::HandleNetworkFailure);

    ULocalPlayer* const Player = GetFirstGamePlayer();
    FindSessions(Player->GetPreferredUniqueNetId()->AsShared(), bIsLAN, true);
}

TArray<FBlueprintSessionResult> UMainGameInstance::GetActiveSession()
{

    TArray<FBlueprintSessionResult> Result;
    const int Length = SessionSearch->SearchResults.Num();

    Result.Reserve(Length);
    Result.SetNum(Length);

    for(int i = 0; i < Length; i++)
    {
      FBlueprintSessionResult Item;
      Item.OnlineResult = SessionSearch->SearchResults[i];
      Result[i] = Item;
    }
    return Result;
}

void UMainGameInstance::JoinOnlineGame(FName SessionName)
{
    CurrentSessionName = SessionName;
    ULocalPlayer* const Player = GetFirstGamePlayer();

    if (SessionSearch->SearchResults.Num() > 0)
    {
        for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
        {
            if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId() && FName(*SessionSearch->SearchResults[i].Session.OwningUserName) == SessionName)
            {
                  ConnectToSession(Player->GetPreferredUniqueNetId()->AsShared(), SessionName, SessionSearch->SearchResults[i]);
                  break;   
            }
        }
    }	
}

void UMainGameInstance::DestroySessionAndLeaveGame() const
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid())
        {
            Sessions->DestroySession(CurrentSessionName, OnDestroySessionCompleteDelegate);
        }
    }
}

void UMainGameInstance::HandleNetworkFailure(UWorld *World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString &ErrorString)
{
    if(IOnlineSubsystem::Get(SubsystemName)->GetSessionInterface()->GetNamedSession(CurrentSessionName))
    {
        UE_LOG(LogOnline, Warning, TEXT("Host disconnected"));
        DestroySessionAndLeaveGame();
    }
}
