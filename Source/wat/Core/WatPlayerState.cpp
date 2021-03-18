#include "wat/Core/WatPlayerState.h"
#include "WatGameInstance.h"
#include "GameFramework/GameStateBase.h"


void AWatPlayerState::OnDeactivated()
{
	if(!HasLocalNetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client %s disconnected"), *this->GetPlayerName());
		auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
		GameInstance->DestroySessionAndLeaveGame();
	}

}
