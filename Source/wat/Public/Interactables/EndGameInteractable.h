// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "EndGameInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AEndGameInteractable : public AInteractable
{
	GENERATED_BODY()
	
protected:
	virtual void Interact() override;


public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=UI)
	TSubclassOf<UUserWidget> EndGameWidgetTemplate;
	UPROPERTY()
	UUserWidget* EndGameWidget;
};