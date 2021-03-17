// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "EndGameInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AEndGameInteractable : public AInteractable
{
	GENERATED_BODY()

	UPROPERTY()
	class AWatGameState* GameState;
	
	virtual void BeginPlay() override;
	
	virtual void OnInteraction() override;
};
