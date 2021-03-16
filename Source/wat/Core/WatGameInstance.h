// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WatGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHostListUpdate);

/**
 * 
 */
UCLASS()
class WAT_API UWatGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UWatGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void BeginDestroy() override;

	
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);
	bool ConnectToSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);



	//Delegates are used to tell us when an event (creation for example) has finished
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;



	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;


	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;


	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	bool StartOnlineGame(FName GameSessionName, bool bIsLan);
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGames(bool bIsLAN);
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	TArray<FBlueprintSessionResult> GetActiveSession();
	UPROPERTY(BlueprintAssignable, Category = "Network|Test")
	FOnHostListUpdate OnHostListUpdate;
	
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame(FName GameSessionName);
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
    void DestroySessionAndLeaveGame() const;
	
    void HandleNetworkFailure(UWorld * World, UNetDriver * NetDriver, ENetworkFailure::Type FailureType, const FString & ErrorString);
	
private:
  FName SubsystemName;
  FName CurrentSessionName;
public:
	bool bIsInTutorialMode;
	bool bHostIsScientist;
#if WITH_EDITOR

	bool bIsHost;
#endif

};
