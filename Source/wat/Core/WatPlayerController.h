// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WatPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AWatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWatPlayerController();

	UFUNCTION(Server, Reliable)
	void PossessRPC(APawn* PawnToPossess, bool bEnableInput = false);

	UFUNCTION(Server, Reliable)
	void NotifyPossessionEnd(class ABlendableInteractable* Interactable);

	UFUNCTION(Server, Reliable)
	void BroadcastInteractionEnd(class AWatPlayerCharacter* InteractingPlayer, class ABlendableInteractable* Interactable);
	
	UFUNCTION(Server, Reliable)
	void NotifyObjectiveCompleted(class AObjective* Objective);

	UFUNCTION(Server,Reliable)
	void UpdateBunkerPlayerCount(const int Value);

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

	bool bIsInMiniGame;

	UPROPERTY()
	class UMiniGameWidget* MiniGameWidget;

	UPROPERTY()
	class UCameraDeskHUDWidget* CameraDeskHUDWidget;

	UPROPERTY()
	class UObjectivesHUDWidget* ObjectivesHUDWidget;

	UPROPERTY()
	class UTutoMessageWidget* TutoMessageWidget;

	UPROPERTY()
	class UMainHUDWidget* MainHUDWidget;
	
	UPROPERTY()
	UUserWidget* DebugGridWidget;

	UPROPERTY(BlueprintReadWrite)
	class UPauseWidget* PauseWidget;

	bool bIsUsingController;
};
