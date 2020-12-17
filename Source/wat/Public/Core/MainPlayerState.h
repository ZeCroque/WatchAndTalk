#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interactables/EndGameInteractable.h"

#include "MainPlayerState.generated.h"

UCLASS()
class WAT_API AMainPlayerState final : public APlayerState
{
    GENERATED_BODY()
    
public:
    UFUNCTION(NetMulticast, Reliable, WithValidation)
    void KillPlayer();
    UFUNCTION(Server, Reliable)
    void CheckIfGameShouldEnd(AEndGameInteractable* Interactable);
    UFUNCTION(NetMulticast, Reliable)
    void EndGame();
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> EndGameWidgetTemplate;

    UFUNCTION(Server, Reliable, WithValidation)
    void Possess(class APlayerController* ClientController, APawn* Pawn);

    virtual void OnDeactivated() override;
};
