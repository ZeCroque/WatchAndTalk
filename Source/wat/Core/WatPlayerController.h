// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "wat/Widgets/QTEWidget.h"
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
	void PossessRPC(APawn* PawnToPossess);

	virtual void BeginPlay() override;

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

	bool bIsInMiniGame;

	UPROPERTY()
	UQTEWidget* Widget;

	UPROPERTY()
	UUserWidget* DebugGridWidget;

	UFUNCTION(Server,Reliable)
	void UpdateBunkerPlayerCount(const int Value);


	bool bIsUsingController;
};
