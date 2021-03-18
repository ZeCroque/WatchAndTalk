// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/Widgets/MiniGames/MiniGameWidget.h"
#include "KeyBasedMiniGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UKeyBasedMiniGameWidget : public UMiniGameWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= "WAT KeyBasedMinigame")
    TArray<FName> PossibleActionsName;

	UPROPERTY(EditAnywhere, Category= "WAT KeyBasedMinigame")
    class USoundCue* KeySound;
};
