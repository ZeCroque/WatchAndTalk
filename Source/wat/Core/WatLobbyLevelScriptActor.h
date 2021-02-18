// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "WatLobbyLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AWatLobbyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
        virtual void PostInitializeComponents() override;
};
