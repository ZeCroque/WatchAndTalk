// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/GameplayActors/Interactables/Interactable.h"
#include "StartChooserInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AStartChooserInteractable : public AInteractable
{
	GENERATED_BODY()

	virtual void OnInteraction() override;
};
