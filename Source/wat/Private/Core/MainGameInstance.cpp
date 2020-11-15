// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameInstance.h"
#include "OnlineSessionSettings.h"
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
    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	
	
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
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

    if (OnlineSub)
    {

        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid() && UserId.IsValid())
        {

            SessionSearch = MakeShareable(new FOnlineSessionSearch());

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

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

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

        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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

    IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
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

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
        UGameplayStatics::OpenLevel(GetWorld(), "Level_POC", true, "listen");
    }
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result));

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
					UGameplayStatics::OpenLevel(World, "Main_Menu_Level", true);
				}
            }
        }
    }
}

bool UMainGameInstance::StartOnlineGame(FName GameSessionName, bool bIsLan)
{
    ULocalPlayer* const Player = GetFirstGamePlayer();
	return HostSession(Player->GetPreferredUniqueNetId()->AsShared(), GameSessionName, bIsLan, true, 2);
}

void UMainGameInstance::FindOnlineGames()
{
    ULocalPlayer* const Player = GetFirstGamePlayer();
    FindSessions(Player->GetPreferredUniqueNetId()->AsShared(), true, true);
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
    ULocalPlayer* const Player = GetFirstGamePlayer();

    FOnlineSessionSearchResult SearchResult;

    if (SessionSearch->SearchResults.Num() > 0)
    {
        for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
        {
            if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
            {
                SearchResult = SessionSearch->SearchResults[i];
                ConnectToSession(Player->GetPreferredUniqueNetId()->AsShared(), SessionName, SearchResult);
                break;
            }
        }
    }	
}

void UMainGameInstance::DestroySessionAndLeaveGame() const
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid())
        {
           // Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
            Sessions->DestroySession(GameSessionName, OnDestroySessionCompleteDelegate);
        }
    }
}
