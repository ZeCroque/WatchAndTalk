// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TextRenderInteractable.h"
#include "wat/GameplayActors/Interactables/Interactable.h"
#include "StartChooserInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AStartChooserInteractable : public ATextRenderInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="WAT Lobby")
	FString QuickStartString;

	UPROPERTY(EditAnywhere, Category="WAT Lobby")
	FString TutorialString;
	
protected:
	UPROPERTY()
	class AWatGameState* GameState;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnInteraction() override;
};
