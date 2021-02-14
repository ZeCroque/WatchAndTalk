// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "WatLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class AWatLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="WAT Objectives")
	class UDataTable* ObjectiveDataTable;
	
protected:
	virtual void PostInitializeComponents() override;
};
