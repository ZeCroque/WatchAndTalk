// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "OctoInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AOctoInteractable : public AInteractable
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
	virtual void OnInteraction() override;

	virtual bool CanInteract(AWatPlayerCharacter* Player) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bActivated;
};
