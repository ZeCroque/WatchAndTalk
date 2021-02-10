#include "wat/Core/WatPlayerState.h"


#include "wat/Core/WatSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void AWatPlayerState::OnDeactivated()
{
    if(!HasLocalNetOwner())
    {
      UE_LOG(LogTemp, Warning, TEXT("Client %s disconnected"), *this->GetPlayerName());
      const auto* NetworkSettingsWat = GetDefault<UWatSettings>();
      UGameplayStatics::OpenLevel(GetWorld(), *NetworkSettingsWat->MainMenuLevel.GetAssetName(), true);
    }

}
