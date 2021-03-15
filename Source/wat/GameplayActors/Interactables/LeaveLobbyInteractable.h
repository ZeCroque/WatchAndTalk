// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextRenderInteractable.h"
#include "LeaveLobbyInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ALeaveLobbyInteractable : public ATextRenderInteractable
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="WAT Lobby")
	FString NotReadyString;

	UPROPERTY(EditAnywhere, Category="WAT Lobby")
	FString ReadyString;

protected:
	UFUNCTION(NetMulticast, Reliable)
	void SetTextRenderToReadyString();
	
	virtual void BeginPlay() override;
	
	virtual void OnInteraction() override;

private:

	TArray<uint32> ReadyPlayersIds;
};
