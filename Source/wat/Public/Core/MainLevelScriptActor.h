// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MainLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	void PostInitializeComponents() override;
};
