// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineIdentityWat.h"
#include "Misc/CommandLine.h"
#include "Misc/Guid.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"
#include "OnlineError.h"

bool FUserOnlineAccountWat::GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	const FString* FoundAttr = AdditionalAuthData.Find(AttrName);
	if (FoundAttr != nullptr)
	{
		OutAttrValue = *FoundAttr;
		return true;
	}
	return false;
}

bool FUserOnlineAccountWat::GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	const FString* FoundAttr = UserAttributes.Find(AttrName);
	if (FoundAttr != nullptr)
	{
		OutAttrValue = *FoundAttr;
		return true;
	}
	return false;
}

bool FUserOnlineAccountWat::SetUserAttribute(const FString& AttrName, const FString& AttrValue)
{
	const FString* FoundAttr = UserAttributes.Find(AttrName);
	if (FoundAttr == nullptr || *FoundAttr != AttrValue)
	{
		UserAttributes.Add(AttrName, AttrValue);
		return true;
	}
	return false;
}

inline FString GenerateRandomUserId(int32 LocalUserNum)
{
	FString HostName;
	if (!ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetHostName(HostName))
	{
		// could not get hostname, use address
		bool bCanBindAll;
		const TSharedPtr<class FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
		HostName = Addr->ToString(false);
	}

	const bool bForceUniqueId = FParse::Param( FCommandLine::Get(), TEXT( "StableNullID" ) );
	
	if ( ( GIsFirstInstance || bForceUniqueId ) && !GIsEditor )
	{
		// When possible, return a stable user id
		return FString::Printf( TEXT( "%s-%s" ), *HostName, *FPlatformMisc::GetLoginId().ToUpper() );
	}

	// If we're not the first instance (or in the editor), return truly random id
	return FString::Printf( TEXT( "%s-%s" ), *HostName, *FGuid::NewGuid().ToString() );
}

bool FOnlineIdentityWat::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	FString ErrorStr;
	TSharedPtr<FUserOnlineAccountWat> UserAccountPtr;

	// valid local player index
	if (LocalUserNum < 0 || LocalUserNum >= MAX_LOCAL_PLAYERS)
	{
		ErrorStr = FString::Printf(TEXT("Invalid LocalUserNum=%d"), LocalUserNum);
	}
	else if (AccountCredentials.Id.IsEmpty())
	{
		ErrorStr = TEXT("Invalid account id, string empty");
	}
	else
	{
		TSharedPtr<const FUniqueNetId>* UserId = UserIds.Find(LocalUserNum);
		if (UserId == nullptr)
		{
			const FString RandomUserId = GenerateRandomUserId(LocalUserNum);

			const FUniqueNetIdWat NewUserId(RandomUserId);
			UserAccountPtr = MakeShareable(new FUserOnlineAccountWat(RandomUserId));
			UserAccountPtr->UserAttributes.Add(USER_ATTR_ID, RandomUserId);

			// update/add cached entry for user
			UserAccounts.Add(NewUserId, UserAccountPtr.ToSharedRef());

			// keep track of user ids for local users
			UserIds.Add(LocalUserNum, UserAccountPtr->GetUserId());
		}
		else
		{
			const FUniqueNetIdWat* UniqueIdStr = reinterpret_cast<const FUniqueNetIdWat*>(UserId->Get());
			TSharedRef<FUserOnlineAccountWat>* TempPtr = UserAccounts.Find(*UniqueIdStr);
			check(TempPtr);
			UserAccountPtr = *TempPtr;
		}
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("Login request failed. %s"), *ErrorStr);
		TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdWat(), ErrorStr);
		return false;
	}

	TriggerOnLoginCompleteDelegates(LocalUserNum, true, *UserAccountPtr->GetUserId(), ErrorStr);
	return true;
}

bool FOnlineIdentityWat::Logout(int32 LocalUserNum)
{
	const TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		// remove cached user account
		UserAccounts.Remove(FUniqueNetIdWat(*UserId));
		// remove cached user id
		UserIds.Remove(LocalUserNum);
		// not async but should call completion delegate anyway
		TriggerOnLogoutCompleteDelegates(LocalUserNum, true);

		return true;
	}
	else
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("No logged in user found for LocalUserNum=%d."),
			LocalUserNum);
		TriggerOnLogoutCompleteDelegates(LocalUserNum, false);
	}
	return false;
}

bool FOnlineIdentityWat::AutoLogin(int32 LocalUserNum)
{
	FString LoginStr;
	FString PasswordStr;
	FString TypeStr;

	FParse::Value(FCommandLine::Get(), TEXT("AUTH_LOGIN="), LoginStr);
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), PasswordStr);
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), TypeStr);

	const bool bEnableWarning = LoginStr.Len() > 0 || PasswordStr.Len() > 0 || TypeStr.Len() > 0;
	
	if (!LoginStr.IsEmpty())
	{
		if (!PasswordStr.IsEmpty())
		{
			if (!TypeStr.IsEmpty())
			{
				return Login(0, FOnlineAccountCredentials(TypeStr, LoginStr, PasswordStr));
			}
			else if (bEnableWarning)
			{
				UE_LOG_ONLINE_IDENTITY(Warning, TEXT("AutoLogin missing AUTH_TYPE=<type>."));
			}
		}
		else if (bEnableWarning)
		{
			UE_LOG_ONLINE_IDENTITY(Warning, TEXT("AutoLogin missing AUTH_PASSWORD=<password>."));
		}
	}
	else if (bEnableWarning)
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("AutoLogin missing AUTH_LOGIN=<login id>."));
	}

	return false;
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityWat::GetUserAccount(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> Result;

	const FUniqueNetIdWat StringUserId(UserId);
	const TSharedRef<FUserOnlineAccountWat>* FoundUserAccount = UserAccounts.Find(StringUserId);
	if (FoundUserAccount != nullptr)
	{
		Result = *FoundUserAccount;
	}

	return Result;
}

TArray<TSharedPtr<FUserOnlineAccount> > FOnlineIdentityWat::GetAllUserAccounts() const
{
	TArray<TSharedPtr<FUserOnlineAccount> > Result;
	
	for (TMap<FUniqueNetIdWat, TSharedRef<FUserOnlineAccountWat>>::TConstIterator It(UserAccounts); It; ++It)
	{
		Result.Add(It.Value());
	}

	return Result;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityWat::GetUniquePlayerId(int32 LocalUserNum) const
{
	const TSharedPtr<const FUniqueNetId>* FoundId = UserIds.Find(LocalUserNum);
	if (FoundId != nullptr)
	{
		return *FoundId;
	}
	return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityWat::CreateUniquePlayerId(uint8* Bytes, int32 Size)
{
	if (Bytes != nullptr && Size > 0)
	{
		const FString StrId(Size, reinterpret_cast<TCHAR*>(Bytes));
		return MakeShareable(new FUniqueNetIdWat(StrId));
	}
	return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityWat::CreateUniquePlayerId(const FString& Str)
{
	return MakeShareable(new FUniqueNetIdWat(Str));
}

ELoginStatus::Type FOnlineIdentityWat::GetLoginStatus(int32 LocalUserNum) const
{
	const TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		return GetLoginStatus(*UserId);
	}
	return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityWat::GetLoginStatus(const FUniqueNetId& UserId) const 
{
	const TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(UserId);
	if (UserAccount.IsValid() &&
		UserAccount->GetUserId()->IsValid())
	{
		return ELoginStatus::LoggedIn;
	}
	return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityWat::GetPlayerNickname(int32 LocalUserNum) const
{
	const TSharedPtr<const FUniqueNetId> UniqueId = GetUniquePlayerId(LocalUserNum);
	if (UniqueId.IsValid())
	{
		return UniqueId->ToString();
	}

	return TEXT("NullUser");
}

FString FOnlineIdentityWat::GetPlayerNickname(const FUniqueNetId& UserId) const
{
	return UserId.ToString();
}

FString FOnlineIdentityWat::GetAuthToken(int32 LocalUserNum) const
{
	const TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		const TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(*UserId);
		if (UserAccount.IsValid())
		{
			return UserAccount->GetAccessToken();
		}
	}
	return FString();
}

void FOnlineIdentityWat::RevokeAuthToken(const FUniqueNetId& UserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate)
{
	UE_LOG_ONLINE_IDENTITY(Display, TEXT("FOnlineIdentityNull::RevokeAuthToken not implemented"));
	TSharedRef<const FUniqueNetId> UserIdRef(UserId.AsShared());
	WatSubsystem->ExecuteNextTick([UserIdRef, Delegate]()
	{
		Delegate.ExecuteIfBound(*UserIdRef, FOnlineError(FString(TEXT("RevokeAuthToken not implemented"))));
	});
}

FOnlineIdentityWat::FOnlineIdentityWat(FOnlineSubsystemWat* InSubsystem)
	: WatSubsystem(InSubsystem)
{
	// autologin the 0-th player
	FOnlineIdentityWat::Login(0, FOnlineAccountCredentials(TEXT("DummyType"), TEXT("DummyUser"), TEXT("DummyId")) );
}

FOnlineIdentityWat::~FOnlineIdentityWat()
{
}

void FOnlineIdentityWat::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(UserId, Privilege, static_cast<uint32>(EPrivilegeResults::NoFailures));
}

FPlatformUserId FOnlineIdentityWat::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const
{
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		auto CurrentUniqueId = GetUniquePlayerId(i);
		if (CurrentUniqueId.IsValid() && (*CurrentUniqueId == UniqueNetId))
		{
			return i;
		}
	}

	return PLATFORMUSERID_NONE;
}

FString FOnlineIdentityWat::GetAuthType() const
{
	return TEXT("");
}
