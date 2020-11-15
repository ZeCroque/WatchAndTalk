// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AMainPlayerState final : public APlayerState
{
    GENERATED_BODY()
    
public:
    UFUNCTION(NetMulticast, Reliable, WithValidation)
    void KillPlayer();

    UFUNCTION(Server, Reliable, WithValidation)
    void Possess(class APlayerController* ClientController, APawn* Pawn);
};
