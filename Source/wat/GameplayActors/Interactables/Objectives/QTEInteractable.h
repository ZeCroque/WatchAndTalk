// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "QTEInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AQTEInteractable : public AObjective
{
	GENERATED_BODY()

private:

	virtual void OnNotFinalBlend() override;

	virtual void OnFinalBlend() override;
};
