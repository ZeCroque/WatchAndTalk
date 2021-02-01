// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "QTEWidget.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();

	UFUNCTION(Server, Reliable)
	void PossessRPC(APawn* PawnToPossess);

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	
	bool bIsInMiniGame;
	
	UPROPERTY()
	UQTEWidget* Widget;
	
	UPROPERTY()
	UUserWidget* DebugGridWidget;
};
